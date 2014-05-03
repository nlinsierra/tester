#include "CompileSolution.h"

using namespace std;

// Функция запуска процесса компиляции
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CompileSolution( int CurrentThreadIdx, char CompileCommand[] ) {
	PROCESS_INFORMATION ProcessInf = {0};
	STARTUPINFO StartInf;
	stringstream LogMsg;
	char CompilerMsg[LOG_MSG_LEN] = "";
	HANDLE hPipeRead = NULL, hPipeWrite = NULL;
	DWORD Read;
	SECURITY_ATTRIBUTES SecurityAttr; 
	bool ReturnResult = false;
 
	SecurityAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
    SecurityAttr.bInheritHandle = TRUE; 
    SecurityAttr.lpSecurityDescriptor = NULL; 
	try {
		if (!CreatePipe(&hPipeRead, &hPipeWrite, &SecurityAttr, 0)) throw E_CREATE_PIPE;
		if (!SetHandleInformation( hPipeRead, HANDLE_FLAG_INHERIT, 0)) throw E_CREATE_PIPE;
		ZeroMemory( &StartInf, sizeof(StartInf) );
		StartInf.cb = sizeof(StartInf);
		StartInf.dwFlags |= STARTF_USESTDHANDLES;
		StartInf.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
		StartInf.hStdError = GetStdHandle(STD_ERROR_HANDLE);
		StartInf.hStdOutput = hPipeWrite;
		ZeroMemory( &ProcessInf, sizeof(ProcessInf) );
		// Запускаем процесс компиляции
		/******************************************************************************************************************************/
		if ( CreateProcess(NULL, CompileCommand, NULL, NULL, TRUE, 0, NULL, Threads[CurrentThreadIdx].dir, &StartInf, &ProcessInf) ) {
			int counter = 0;
			int WaitRes = -1;
			DWORD Avail;
			//std::cout << std::endl << "Start compilation" << std::endl;
			//WaitRes = WaitForSingleObject(ProcessInf.hProcess, 3000000);
			while ((WaitRes = WaitForSingleObject( ProcessInf.hProcess, 100 )) == WAIT_TIMEOUT && counter < 300) {
				counter++;
				if (PeekNamedPipe(hPipeRead, NULL, 0, NULL, &Avail, NULL) && Avail) {
					if (!ReadFile(hPipeRead, CompilerMsg, sizeof(CompilerMsg), &Read, NULL)) 
						LogMsg << "Reading pipe error: " << GetLastError() << endl;
					else CompilerMsg[Read] = '\0';
					LogMsg << CompilerMsg << endl;
				}
			}
			if (PeekNamedPipe(hPipeRead, NULL, 0, NULL, &Avail, NULL) && Avail) {
				if (Read != 0 && !ReadFile(hPipeRead, CompilerMsg, sizeof(CompilerMsg), &Read, NULL)) 
					LogMsg << "Reading pipe error: " << GetLastError() << endl;
				else CompilerMsg[Read] = '\0';
				LogMsg << CompilerMsg << endl;
			}
			if (WaitRes == WAIT_TIMEOUT && !TerminateProcess(ProcessInf.hProcess, 0)) {
				if (PeekNamedPipe(hPipeRead, NULL, 0, NULL, &Avail, NULL) && Avail) {
					if (!ReadFile(hPipeRead, CompilerMsg, sizeof(CompilerMsg), &Read, NULL))
						LogMsg << "Reading pipe error: " << GetLastError() << endl;
					else CompilerMsg[Read] = '\0';
					LogMsg << CompilerMsg << endl;
				}
				LogMsg << "Terminating process error: " << GetLastError() << endl;
			}
			
			//cout << endl << "Compilation finished" << endl;
			ReturnResult = true;
			throw E_SUCCESSFULL;
		}
		else throw E_CREATE_COMPILATION_PROCESS;
		/******************************************************************************************************************************/
	}
	catch (int Exception) {
		if (hPipeRead == NULL) CloseHandle(hPipeRead);
		if (hPipeWrite == NULL) CloseHandle(hPipeWrite);
		if (ProcessInf.hThread != NULL) CloseHandle(ProcessInf.hThread);
		if (ProcessInf.hProcess != NULL) CloseHandle(ProcessInf.hProcess);
		if (strcmp(Messages[Exception - 1].Msg, "") != 0)
			LogMsg << Messages[Exception - 1].Msg << ": " << GetLastError() << endl;
		Threads[CurrentThreadIdx].LogMsg += LogMsg.str();
		return ReturnResult;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Функция выполнения компиляции со всеми вспомогательными операциями
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool PerformCompilation(int CurrentThreadIdx) {
	char query[MAX_STR_LEN], CurrentCmd[MAX_STR_LEN];// LogMsg[LOG_MSG_LEN];
	stringstream LogMsg;
	int CompilerId = Threads[CurrentThreadIdx].CurrentSubmit.compiler_id;
	int SubmitId = Threads[CurrentThreadIdx].CurrentSubmit.id;
	char MainFile[MAX_STR_LEN];

	sprintf_s(MainFile, sizeof(MainFile),"%s", Threads[CurrentThreadIdx].CurrentSubmit.main_file);
	sprintf_s(CurrentCmd, sizeof(CurrentCmd), Compilers[CompilerId].CommandFmt, Threads[CurrentThreadIdx].dir, MainFile);
	if (!CompileSolution(CurrentThreadIdx, CurrentCmd)) {
		LogMsg << "Internal tester error" << endl;
		// Записывает результат проверки в базу
		sprintf_s(query, sizeof(query), "update submits set test_result = %d", INTERNAL_ERROR + 1);
		sprintf_s(query, sizeof(query), "%s, test_result_comment = \'Internal tester error\'", query);
		sprintf_s(query, sizeof(query), "%s, memory_used = 0", query);
		sprintf_s(query, sizeof(query), "%s, time_used = 0", query);
		sprintf_s(query, sizeof(query), "%s where id = %d", query, SubmitId);
		if (mysql_query(&Threads[CurrentThreadIdx].mysql,query) != 0) 
			LogMsg << "Update result SQL query error" << endl;
		Threads[CurrentThreadIdx].LogMsg += LogMsg.str();
		return false;
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
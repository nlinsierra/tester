#include "TestSolution.h"

using namespace std;

// Функция настройки перехвата API в процессе-решении
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SetAPIInterception(HANDLE hProcess, char InterceptionLibName[]) {
	LPVOID LoadFunctionAddress = NULL, DLLPathAddress = NULL;
	DWORD oldProtect = 0;
	HANDLE RemoteThread = NULL;
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	try {
		DLLPathAddress = (LPVOID)VirtualAllocEx(hProcess, NULL, strlen(InterceptionLibName), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		
		//VirtualProtectEx(hProcess, (LPVOID)DLLPathAddress, strlen(InterceptionLibName), PAGE_EXECUTE_READWRITE, &oldProtect);
		
		if (DLLPathAddress == NULL) throw E_INTERSEPT_API;
		LoadFunctionAddress = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"),"LoadLibraryA");
		if (LoadFunctionAddress == NULL) throw E_INTERSEPT_API;
		if (!WriteProcessMemory(hProcess, (LPVOID)DLLPathAddress, InterceptionLibName, strlen(InterceptionLibName), NULL)) throw E_INTERSEPT_API;
		
		//VirtualProtectEx(hProcess, (LPVOID)DLLPathAddress, strlen(InterceptionLibName), oldProtect, &oldProtect);

		//FlushInstructionCache(hProcess, (LPVOID)DLLPathAddress, strlen(InterceptionLibName));
		
		RemoteThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadFunctionAddress, (LPVOID)DLLPathAddress, NULL, NULL);
		if (RemoteThread == NULL) throw E_INTERSEPT_API;
		WaitForSingleObject(RemoteThread, INFINITE);		
		CloseHandle(RemoteThread);
		VirtualFreeEx(hProcess, (LPVOID)DLLPathAddress, 0, MEM_RELEASE | MEM_DECOMMIT);
		return true;
	}
	catch (...) {
		if (DLLPathAddress != NULL) VirtualFreeEx(hProcess, DLLPathAddress, 0, MEM_RELEASE);
		return false;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Функция завершения всех процессов объекта Job
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CloseAllForJob(HANDLE job, int CurrentThreadIdx) {
	stringstream LogMsg;

	try {
		if (!TerminateJobObject(job, 0)) throw E_TERMINATE_JOB_OBJECT;
		return true;
	}
	catch (int Exception) {
		LogMsg << Messages[Exception - 1].Msg << ": " << GetLastError();
		Threads[CurrentThreadIdx].LogMsg += LogMsg.str();
		return false;
	}	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Функция интерпретации сообщения порта завершения в вердикт проверки 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int SwitchRunResult(int RunResult, HANDLE job, int CurrentThreadIdx) {
	int Verdict = RUNTIME_ERROR;
	stringstream LogMsg;

	switch (RunResult) {
		case JOB_OBJECT_MSG_END_OF_PROCESS_TIME:				
		case JOB_OBJECT_MSG_END_OF_JOB_TIME:
			Verdict = TIME_LIMIT;
			break;
		case JOB_OBJECT_MSG_PROCESS_MEMORY_LIMIT:
		case JOB_OBJECT_MSG_JOB_MEMORY_LIMIT:
			Verdict = MEMORY_LIMIT;
			break;
		case JOB_OBJECT_MSG_ABNORMAL_EXIT_PROCESS:			
			Verdict = RUNTIME_ERROR;
			break;
		case JOB_OBJECT_MSG_EXIT_PROCESS:
			return ACCEPTED;
		case JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO:
			LogMsg << "No active process" << endl;
			Verdict = INTERNAL_ERROR;
			break;
		case JOB_OBJECT_MSG_ACTIVE_PROCESS_LIMIT:
			LogMsg << "Active process limit exceeded" << endl;
			Verdict = RUNTIME_ERROR;
			break;
		default: 
			LogMsg << "Unexpected error: " << GetLastError() << endl;
			Verdict = INTERNAL_ERROR;
			break;
	}
	Threads[CurrentThreadIdx].LogMsg += LogMsg.str();
	return Verdict;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Функция вычисления размера памяти, используемой процесса
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD GetProcessMemory(HANDLE hProcess) {
	MEMORY_BASIC_INFORMATION mbi;
	BOOL BAddrOK, FreeOK, ReservedOK;
	DWORD MemSize = 0;

	for (DWORD PagesCount = 1; 
		 VirtualQueryEx(hProcess, (LPCVOID)(PagesCount + 1), &mbi, sizeof(mbi)); 
		 PagesCount = (DWORD)mbi.BaseAddress + mbi.RegionSize + 1) {
		BAddrOK =(DWORD)mbi.RegionSize;
		FreeOK = (mbi.State != MEM_FREE);
		ReservedOK = (mbi.State != MEM_RESERVE);
		if ( BAddrOK && FreeOK && ReservedOK ) MemSize += mbi.RegionSize;	
	}	
	return MemSize;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Функция получения размера памяти объекта Job 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SIZE_T GetJobMemoryInformation(int CurrentThreadIdx, HANDLE job) {
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION ExJobInf;
	DWORD ReturnLength;
	stringstream LogMsg;
	SIZE_T JobMemory = 0;

	try {
		if (!QueryInformationJobObject(job, JobObjectExtendedLimitInformation, &ExJobInf, sizeof(ExJobInf), &ReturnLength)) 
			throw E_READ_JOB_MEMORY_INFORMATION;
		JobMemory = ExJobInf.PeakProcessMemoryUsed / KB;
		return JobMemory;
	}
	catch (int Exception) {
		LogMsg << Messages[Exception - 1].Msg << ": " << GetLastError() << endl;
		Threads[CurrentThreadIdx].LogMsg += LogMsg.str();
		return JobMemory;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Функция настройки объекта Job
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SetJobParameters(HANDLE job, HANDLE completion_port, int CurrentThreadIdx) {
	JOBOBJECT_ASSOCIATE_COMPLETION_PORT CPInf;
	stringstream LogMsg;
	
	CPInf.CompletionKey = (PVOID) ((UINT_PTR) 2);
	CPInf.CompletionPort = completion_port;
	try {
		if (job == NULL) 
			throw E_CREATE_JOB_OBJECT;
		if (completion_port == NULL) 
			throw E_CREATE_COMPLETION_PORT;
		if (!SetInformationJobObject(job, JobObjectAssociateCompletionPortInformation, &CPInf, sizeof(CPInf))) 
			throw E_COMPLETION_PORT_ASSOCIATION;
		return true;
	}
	catch (int Exception) {		
		LogMsg << Messages[Exception - 1].Msg << ": " << GetLastError() << endl;
		Threads[CurrentThreadIdx].LogMsg += LogMsg.str();
		return false;
	}	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Функция запуска исполняемого файла текущего решения
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int DoProgram(int problem_id, int CurrentThreadIdx, LONGLONG &ProcessTime, SIZE_T &ProcessMemory) {
	PROCESS_INFORMATION ProcessInf = {0};
	SECURITY_ATTRIBUTES sa;
	STARTUPINFO StartInf;
	DWORD RunResult = -1, MemSize = 0, MaxMemSize = 0;
	char ExePath[MAX_STR_LEN], InterceptionLibName[MAX_STR_LEN];
	stringstream LogMsg;
    ULONG_PTR CompKey;
    LPOVERLAPPED POverlapped;
	LARGE_INTEGER start, end, freq;
	HANDLE job = NULL, completion_port = NULL;
	int ReturnValue = RUNTIME_ERROR; 

	sprintf_s(InterceptionLibName, sizeof(InterceptionLibName), "%s\\APIInterception.dll", CurDir);
	QueryPerformanceFrequency(&freq);
	try {	
		// Выполняем настройку объектов Job и Completion port
		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;
		job = CreateJobObject(&sa, NULL);
		completion_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);		
		if (!SetJobParameters(job, completion_port, CurrentThreadIdx)) throw E_SET_JOB_INFORMATION;
		// Проверяем создание исполняемого файла
		// Если исполняемый файл не создан, значит процесс компиляции завершился с ошибкой
		sprintf_s(ExePath, sizeof(ExePath), "%s\\%s", Threads[CurrentThreadIdx].dir, ExeName);
		// Устанавливаем ограничения на время и память для объекта 'job' 
		if (!SetLimits(job, Problems[problem_id].MaxTime, Problems[problem_id].MaxMem, CurrentThreadIdx)) throw E_SET_LIMITS;   
		ZeroMemory(&StartInf, sizeof(StartInf));
		StartInf.cb = sizeof(StartInf);
		ZeroMemory( &ProcessInf, sizeof(ProcessInf) );    		
		// Запускаем решение на выполнение
		/**************************************************************************************************************************************/
		if (CreateProcess(ExePath, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED | CREATE_BREAKAWAY_FROM_JOB, NULL, Threads[CurrentThreadIdx].dir, &StartInf, &ProcessInf)) {
			//if (!SetAPIInterception(ProcessInf.hProcess, InterceptionLibName)) throw E_INTERSEPT_API;
			if (!AssignProcessToJobObject(job, ProcessInf.hProcess)) throw E_ASSIGN_PROCESS_TO_JOB;		
			// Проверяем память инициализированного процесса до запуска
			ProcessMemory = (SIZE_T)GetProcessMemory(ProcessInf.hProcess) / KB;
			if (ProcessMemory * KB > (SIZE_T)Problems[problem_id].MaxMem) {				 
				ReturnValue = MEMORY_LIMIT; 
				throw E_MEMORY_LIMIT; 
			}			
			// Запускаем главный поток процесса
			if (ResumeThread(ProcessInf.hThread) == FAILED_RESULT) throw E_RESUME_THREAD;
			// Инициализируем таймер для подсчета времени работы процесса
			QueryPerformanceCounter(&start);
			// Запускаем цикл опроса порта завершения (интервал - 10 мс)
			while (!GetQueuedCompletionStatus(completion_port, &RunResult, &CompKey, &POverlapped, TIME_CHECK_INTERVAL) ||
				   RunResult == JOB_OBJECT_MSG_NEW_PROCESS) {
				QueryPerformanceCounter(&end);
				ProcessTime = ((end.QuadPart - start.QuadPart) * 1000) / freq.QuadPart;
				if (ProcessTime > Problems[problem_id].MaxTime) {
					ReturnValue = TIME_LIMIT;
					ProcessMemory += GetJobMemoryInformation(CurrentThreadIdx, job);
					throw E_TIME_LIMIT;
				}
			}
			QueryPerformanceCounter(&end);
			// Вычисляем память и время работы процесса
			ProcessMemory += GetJobMemoryInformation(CurrentThreadIdx, job);
			ProcessTime = ((end.QuadPart - start.QuadPart) * 1000) / freq.QuadPart;	
			// Преобразуем сообщение порта завершения в вердикт проверки
			ReturnValue = SwitchRunResult(RunResult, job, CurrentThreadIdx);
			// Если MEMORY_LIMIT, то добавляем к полученной памяти значение MaxMem
			if (ReturnValue == MEMORY_LIMIT) ProcessMemory += (SIZE_T)Problems[problem_id].MaxMem / KB;
			DWORD ProcessResult;
			if (!GetExitCodeProcess(ProcessInf.hProcess, &ProcessResult) || ProcessResult != 0) {
				ReturnValue = RUNTIME_ERROR;
				throw E_RUNTIME_ERROR;
			}
			throw E_SUCCESSFULL;
		}
		else throw E_CREATE_PROCESS;
		/**************************************************************************************************************************************/
	}
	catch (int Exception) {
		if (Exception != E_SUCCESSFULL && Exception != E_MEMORY_LIMIT && Exception != E_TIME_LIMIT && Exception != E_RUNTIME_ERROR) 
			ReturnValue = INTERNAL_ERROR;
		if (!CloseAllForJob(job, CurrentThreadIdx)) ReturnValue = INTERNAL_ERROR;
		if (WaitForSingleObject(ProcessInf.hProcess, WAIT_INTERVAL) == WAIT_TIMEOUT) { 
			LogMsg << "Process " << ExePath << " is not finished in time" << endl;
			ReturnValue = INTERNAL_ERROR;
		}
		if (job != NULL) CloseHandle(job);
		if (completion_port != NULL) CloseHandle(completion_port);
		if (ProcessInf.hThread != NULL) CloseHandle(ProcessInf.hThread);
		if (ProcessInf.hProcess != NULL) CloseHandle(ProcessInf.hProcess);		
		if (strcmp(Messages[Exception - 1].Msg, "") != 0)
			LogMsg << Messages[Exception - 1].Msg << ": " << GetLastError() << endl;
		Threads[CurrentThreadIdx].LogMsg += LogMsg.str();
		return ReturnValue; 
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Функция считывания содержимого текстового файла в строковую переменную
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
string GetFileToStr(string FileName) {
	string Res = "", LB = "\n";
	unsigned len, pos = 0;
	char * buffer;
	ifstream fin;
	fin.open (FileName, ios::binary );
	fin.seekg (0, ios::end);
	len = (unsigned)fin.tellg();
	fin.seekg (0, ios::beg);
	buffer = new char [len + 1];
	fin.read (buffer,len);
	fin.close();
	buffer[len] = '\0';	
	Res = string(buffer);
	while ((pos = Res.find("\r\n",pos)) != string::npos)
		Res.replace(pos, 2, LB);
	delete [] buffer;
	return Res;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Функция инициализации структуры TestResult значениями
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TestResult SetTestResult(int VerdictId, int TestNum, string FInStr, string FOutStr, string POutStr) {
	TestResult CurrentTestResult;
	if (VerdictId != ACCEPTED) {
		if (FInStr != "") CurrentTestResult.TestIn = GetFileToStr(FInStr);
		if (FOutStr != "") CurrentTestResult.CorrectOut = GetFileToStr(FOutStr);
		if (VerdictId == WRONG_ANSWER || VerdictId == PRESENTATION_ERROR)
			if (POutStr != "") CurrentTestResult.ProgramOut = GetFileToStr(POutStr);
	}

	CurrentTestResult.VerdictId = VerdictId;
	CurrentTestResult.TestNum = TestNum;
	return CurrentTestResult;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Функция проверки текущего решения на тестах
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TestResult CheckSolution(int ProblemId, int CurrentThreadIdx, CompareFunction CompareFiles) {
	TestResult CurrentTestResult = {-1, -1, "", "", ""};
	int TestCount = 1, Verdict = 0;
	char FindFileMask[MAX_STR_LEN], InputPathTo[MAX_STR_LEN], InputPathFrom[MAX_STR_LEN];
	char ProgramOutput[MAX_STR_LEN], CorrectOutput[MAX_STR_LEN];
	stringstream LogMsg;
	WIN32_FIND_DATA FileData;
	LONGLONG MaxTime = 0, CurrentTime = 0;
	SIZE_T MaxMemory = 0, CurrentMemory = 0;
	char ExePath[MAX_STR_LEN];
	char CurTestsPath[MAX_STR_LEN];

	sprintf_s(ExePath, sizeof(ExePath), "%s\\%s", Threads[CurrentThreadIdx].dir, ExeName);
	// Путь к тестам для текущей задачи 
	sprintf_s(CurTestsPath, sizeof(CurTestsPath), "%s\\%s", TestsPath, Problems[ProblemId].Id);
	// Маска входных файлов
	sprintf_s(FindFileMask, sizeof(FindFileMask), "%s\\*.in", CurTestsPath);
	// Путь к файлу input.txt
	sprintf_s(InputPathTo, sizeof(InputPathTo), "%s\\input.txt", Threads[CurrentThreadIdx].dir);
	// Путь к файлу output.txt
	sprintf_s(ProgramOutput, sizeof(ProgramOutput), "%s\\output.txt", Threads[CurrentThreadIdx].dir);
	HANDLE hFile = FindFirstFile(FindFileMask, &FileData);
	try {
		if (_access(ExePath, 0) == -1) {
			CurrentTestResult = SetTestResult(COMPILE_ERROR, TestCount, "", "", "");
			throw E_COMPILE_ERROR;
		}
		if (hFile == INVALID_HANDLE_VALUE) throw E_NO_TESTS;
		// Запускаем цикл проверки решения на имеющихся тестах
		/**************************************************************************************************************************************/
		do {
			sprintf_s(InputPathFrom, sizeof(InputPathFrom), "%s\\%s", CurTestsPath, FileData.cFileName);
			sprintf_s(CorrectOutput, sizeof(CorrectOutput), "%s", InputPathFrom);
			CorrectOutput[strlen(CorrectOutput) - 2] = '\0';
			strcat_s(CorrectOutput, sizeof(CorrectOutput), "out");
			if (!CopyFile(InputPathFrom, InputPathTo, FALSE)) {
				CurrentTestResult = SetTestResult(RUNTIME_ERROR, TestCount, InputPathFrom, CorrectOutput, "");
				throw E_COPY_INPUT_FILE;				
			}
			CurrentTime = 0;
			CurrentMemory = 0;
			// Запускаем exe-файл решения на текущем тесте
			/*******************************************************************************/
			Verdict = DoProgram(ProblemId, CurrentThreadIdx, CurrentTime, CurrentMemory);
			/*******************************************************************************/
			if (CurrentTime > MaxTime) MaxTime = CurrentTime;
			if (CurrentMemory > MaxMemory) MaxMemory = CurrentMemory;
			Threads[CurrentThreadIdx].CurrentSubmit.time_used = MaxTime;
			Threads[CurrentThreadIdx].CurrentSubmit.memory_used = MaxMemory;	
			if (Verdict != ACCEPTED) {
				CurrentTestResult = SetTestResult(Verdict, TestCount, InputPathFrom, CorrectOutput, ProgramOutput);	
				throw E_SUCCESSFULL;
			}
			if (_access(ProgramOutput, 0) == -1) {
				CurrentTestResult = SetTestResult(WRONG_ANSWER, TestCount, InputPathFrom, CorrectOutput, "");				
				throw E_SUCCESSFULL;
			}
			int CurCmpResult = CompareFiles(ProgramOutput, CorrectOutput);
			if (CurCmpResult == -1) {
				CurrentTestResult = SetTestResult(WRONG_ANSWER, TestCount, InputPathFrom, CorrectOutput, ProgramOutput);
				throw E_SUCCESSFULL;
			}
			if (CurCmpResult == 1) {
				CurrentTestResult = SetTestResult(PRESENTATION_ERROR, TestCount, InputPathFrom, CorrectOutput, ProgramOutput);
				throw E_SUCCESSFULL;
			}
			if (CurCmpResult != 0) {
				CurrentTestResult = SetTestResult(WRONG_ANSWER, TestCount, InputPathFrom, CorrectOutput, ProgramOutput);
				throw E_SUCCESSFULL;
			}
			if (!DeleteFile(ProgramOutput)) //sprintf_s(LogMsg, sizeof(LogMsg), "%sCan't delete output file: %d\n", LogMsg, GetLastError());
				LogMsg << "Can't delete output file: " << GetLastError() << endl;
			++TestCount;
		} while (FindNextFile(hFile, &FileData));		
		CurrentTestResult = SetTestResult(ACCEPTED, TestCount, "", "", "");
		throw E_SUCCESSFULL;
		/**************************************************************************************************************************************/
	}
	catch (int Exception) {
		switch (Exception) {
			case E_COPY_INPUT_FILE:
				LogMsg << Messages[Exception - 1].Msg << ": " << GetLastError() << endl;
				SetTestResult(INTERNAL_ERROR, TestCount, "", "", "");
				break;
			case E_SUCCESSFULL:
			case E_COMPILE_ERROR:
				LogMsg << "Time used: " << MaxTime << " ms" << endl;
				LogMsg << "Memory used: " << MaxMemory << " Kb" << endl;
				if (Exception == E_COMPILE_ERROR) break;
				if (!FindClose(hFile)) 
					LogMsg << "Can't close current find operation: " << GetLastError() << endl;
				break;
			default: 
				SetTestResult(INTERNAL_ERROR, TestCount, "", "", "");
				break;
		}
		Threads[CurrentThreadIdx].LogMsg += LogMsg.str();
		return CurrentTestResult;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Функция запуска проверки текущего решения
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TestCurrentSolution(int CurrentThreadIdx) {
	HINSTANCE hLib = NULL;
	char query[MAX_STR_LEN], LibPath[MAX_STR_LEN], VerdictMsg[MAX_STR_LEN];
	stringstream LogMsg;
	CompareFunction CompareFiles;
	TestResult Result;
	int ProblemId = Threads[CurrentThreadIdx].CurrentSubmit.problem_id;
	int SubmitId = Threads[CurrentThreadIdx].CurrentSubmit.id;
	LONGLONG TimeUsed = Threads[CurrentThreadIdx].CurrentSubmit.time_used;
	SIZE_T MemoryUsed = Threads[CurrentThreadIdx].CurrentSubmit.memory_used;
	bool ReturnResult = false;
	
	sprintf_s(LibPath, sizeof(LibPath), "%s\\%s\\%s", TestsPath, Problems[ProblemId].Id, Problems[ProblemId].Checker);
	try {
		hLib = LoadLibrary(LibPath);
		if (hLib != NULL) {
			CompareFiles = (CompareFunction)GetProcAddress((HMODULE)hLib, "CompareFiles");
			if (CompareFiles != NULL) 
				// Запускаем проверку текущего решения на тестах
				/******************************************************************/
				Result = CheckSolution(ProblemId, CurrentThreadIdx, CompareFiles);	
				/******************************************************************/
			else {			
				FreeLibrary(hLib);
				Result.VerdictId = INTERNAL_ERROR;
				sprintf_s(VerdictMsg, sizeof(VerdictMsg), "%s", Verdicts[Result.VerdictId].content);
				LogMsg << VerdictMsg << endl;
				throw E_READ_DLL_FUNCTION;
			}
			FreeLibrary(hLib);
			TimeUsed = Threads[CurrentThreadIdx].CurrentSubmit.time_used;
			MemoryUsed = Threads[CurrentThreadIdx].CurrentSubmit.memory_used;
			// Записываем результат проверки в лог
			/***********************************************************************************************************************/
			if (Result.VerdictId == FAILED_RESULT) 
				sprintf_s(VerdictMsg, sizeof(VerdictMsg), "ERROR! NO TESTS FOR THIS PROBLEM!");
			else if (Result.VerdictId == COMPILE_ERROR) 
				sprintf_s(VerdictMsg, sizeof(VerdictMsg), "Compilation error"); 
			else if (Result.VerdictId != ACCEPTED) 
				sprintf_s(VerdictMsg, sizeof(VerdictMsg), "%s on test %d", Verdicts[Result.VerdictId].content, Result.TestNum); 
			else 
				sprintf_s(VerdictMsg, sizeof(VerdictMsg), "%s", Verdicts[Result.VerdictId].content); 
			LogMsg << VerdictMsg << endl;
			/***********************************************************************************************************************/			
			ReturnResult = true;
			throw E_SUCCESSFULL;
		}
		else {
			Result.VerdictId = INTERNAL_ERROR;
			sprintf_s(VerdictMsg, sizeof(VerdictMsg), "%s", Verdicts[Result.VerdictId].content);
			LogMsg << VerdictMsg << endl;
			throw E_LIBRARY_ACCESS;
		}
	}
	catch (int Exception) {
		// Записываем результат проверки в базу
		/***********************************************************************************************************************/
		sprintf_s(query, sizeof(query), "update submits set test_result = %d", Result.VerdictId + 1);
		sprintf_s(query, sizeof(query), "%s, test_result_comment = \'%s\'", query, VerdictMsg);
		sprintf_s(query, sizeof(query), "%s, memory_used = %d", query, MemoryUsed);
		sprintf_s(query, sizeof(query), "%s, test_in = \'%s\'", query, Result.TestIn.c_str());
		sprintf_s(query, sizeof(query), "%s, test_out = \'%s\'", query, Result.CorrectOut.c_str());
		sprintf_s(query, sizeof(query), "%s, program_out = \'%s\'", query, Result.ProgramOut.c_str());
		sprintf_s(query, sizeof(query), "%s, time_used = %I64d ", query, TimeUsed);
		sprintf_s(query, sizeof(query), "%s where id = %d", query, SubmitId);
		if (mysql_query(&Threads[CurrentThreadIdx].mysql,query) != 0) { 
			LogMsg << "Update result SQL query error" << endl;
			LogMsg << mysql_error(&Threads[CurrentThreadIdx].mysql) << endl;
		}
		/***********************************************************************************************************************/
		sprintf_s(query, sizeof(query), "delete from current_testing_users where user_id = %d", Threads[CurrentThreadIdx].submit_user_id);
		if (mysql_query(&Threads[CurrentThreadIdx].mysql, query) != 0) { 
			LogMsg << "Delete testing user id SQL query error" << endl;
			LogMsg << mysql_error(&Threads[CurrentThreadIdx].mysql) << endl;
		}
		if (strcmp(Messages[Exception - 1].Msg, "") != 0)
			LogMsg << Messages[Exception - 1].Msg  << ": " << GetLastError() << endl;
		Threads[CurrentThreadIdx].LogMsg += LogMsg.str();
		return ReturnResult;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
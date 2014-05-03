#include "Run.h"

using namespace std;

// ���� ���������� ����������
bool Exit = false; 

// ����������� �������
HANDLE hThreads[THREAD_COUNT + 1] = {NULL};

// ���������� � �������
ThreadsInformaition Threads[THREAD_COUNT];

// ���������� �������������� �������
int ThreadsIdx = 0;

// ������� �������� ����� ������� ��� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CreateMainFile(int CurrentThreadIdx) {
	char fname[MAX_STR_LEN];
	sprintf_s(fname, sizeof(fname), "%s\\%s", Threads[CurrentThreadIdx].dir, Threads[CurrentThreadIdx].CurrentSubmit.main_file);
	ofstream fout(fname);
	fout << Threads[CurrentThreadIdx].CurrentSubmit.solution;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// ������� �������� ���� ������ �� ������� ���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DelAllFilesInThreadDir(int CurrentThreadIdx) {
	char FPath[MAX_STR_LEN], FindFileMask[MAX_STR_LEN];
	HANDLE hFile;
	WIN32_FIND_DATA FileData;
	stringstream LogMsg;

	sprintf_s(FindFileMask, sizeof(FindFileMask), "%s\\*.*", Threads[CurrentThreadIdx].dir);
	hFile = FindFirstFile(FindFileMask, &FileData);
	if (hFile == INVALID_HANDLE_VALUE) return;
	do {
		if (FileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) continue;
		sprintf_s(FPath, sizeof(FPath), "%s\\%s", Threads[CurrentThreadIdx].dir, FileData.cFileName);
		if (!DeleteFile(FPath)) 
			LogMsg << "Can't delete file " << FPath << ": " << GetLastError() << endl;
	} while (FindNextFile(hFile, &FileData));
	if (!FindClose(hFile)) 		
		LogMsg << "Can't close current find operation: " << GetLastError() << endl;
	Threads[CurrentThreadIdx].LogMsg += LogMsg.str();	
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// ������� ��������� ����������� ������� ������������� ������� ����������� ������� 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool GetCurrentSubmitUsers(int CurrentThreadIdx, vector<int> &CurrentSubmitUsers) {
	char query[MAX_STR_LEN];
	MYSQL_RES *sql_res;
	MYSQL_ROW row;

	CurrentSubmitUsers.resize(0);
	sprintf_s(query, sizeof(query), "select user_id from current_testing_users");
	if (mysql_query(&Threads[CurrentThreadIdx].mysql, query) != 0) return false;
	sql_res = mysql_store_result(&Threads[CurrentThreadIdx].mysql);	
	for (int i = 0; i < sql_res->row_count; ++i) {
		row = mysql_fetch_row(sql_res);
		CurrentSubmitUsers.push_back(atoi(row[0]));
	}
	mysql_free_result(sql_res);
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// ������� ��������� ���������� ������� �� ����
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool GetNewSolution(int CurrentThreadIdx) {
	string query;
	stringstream query_stream, LogMsg;
	MYSQL_RES *sql_res;
	MYSQL_ROW row;
	vector<int> CurrentSubmitUsers;

	try {
		// ��������� ������ �� ������� �������������� �������
		query = "lock tables submits write, current_testing_users write";
		if (mysql_query(&Threads[CurrentThreadIdx].mysql, query.c_str()) != 0) throw E_LOCK_TABLE_SQL;
		// ��������� � ������ ������� �������������, ��� ������� ����������� � ������ ������
		if (!GetCurrentSubmitUsers(CurrentThreadIdx, CurrentSubmitUsers)) throw E_SELECT_SOLUTION_SQL;
		query_stream << "select id, user_id, problem_id, compiler_id, solution from submits where is_checked = 0 ";
		for (unsigned i = 0; i < CurrentSubmitUsers.size(); ++i) 
			query_stream << "and user_id <> " << CurrentSubmitUsers[i] << " ";

		query_stream << "limit 1";
		query = query_stream.str();
		if (mysql_query(&Threads[CurrentThreadIdx].mysql, query.c_str()) != 0) throw E_SELECT_SOLUTION_SQL;
		sql_res = mysql_store_result(&Threads[CurrentThreadIdx].mysql);
		// ���� ������������� ������� ���, �� ���������� ����� ����
		if (sql_res->row_count == 0) {
			mysql_free_result(sql_res);
			throw E_NO_SOLUTION;
		}
		// ���� �������� ����� �������, �� ��������� ��� � ��������� CurrentSubmit
		row = mysql_fetch_row(sql_res);			
		Threads[CurrentThreadIdx].CurrentSubmit.id = atoi(row[0]);
		query_stream.str(string());
		query_stream << "insert into current_testing_users values (" << row[1] << ")";
		query = query_stream.str();
		if (mysql_query(&Threads[CurrentThreadIdx].mysql, query.c_str()) != 0) throw E_SELECT_SOLUTION_SQL;
		Threads[CurrentThreadIdx].submit_user_id = atoi(row[1]);
		Threads[CurrentThreadIdx].CurrentSubmit.problem_id = atoi(row[2]) - 1;
		Threads[CurrentThreadIdx].CurrentSubmit.compiler_id = atoi(row[3]) - 1;
		strcpy_s(Threads[CurrentThreadIdx].CurrentSubmit.main_file,Compilers[atoi(row[3]) - 1].MainFile);
		strcpy_s(Threads[CurrentThreadIdx].CurrentSubmit.solution,row[4]);
		Threads[CurrentThreadIdx].CurrentSubmit.memory_used = 0;
		Threads[CurrentThreadIdx].CurrentSubmit.time_used = 0;
		mysql_free_result(sql_res);
		// ��������� ������ ������� �� ��������� ��������� �������������� ������� � "���������"
		query_stream.str(string());
		query_stream << "update submits set is_checked = 1 where id = " << Threads[CurrentThreadIdx].CurrentSubmit.id;
		query = query_stream.str();
		if (mysql_query(&Threads[CurrentThreadIdx].mysql, query.c_str()) == 0) mysql_query(&Threads[CurrentThreadIdx].mysql, "unlock tables");
		else throw E_UPDATE_CHECK_STATE;
		return true;
	}
	catch (int Exception) {
		if (Exception != E_LOCK_TABLE_SQL) mysql_query(&Threads[CurrentThreadIdx].mysql, "unlock tables");
		if (Exception != E_NO_SOLUTION) LogMsg << Messages[Exception - 1].Msg << endl;
		LogMsg << mysql_error(&Threads[CurrentThreadIdx].mysql) << endl;
		Threads[CurrentThreadIdx].LogMsg += LogMsg.str();
		Sleep(DELAY);
		return false;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// ��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD WINAPI MainThreadsFunction(PVOID Params) {	
	int CurrentThreadIdx = 0;
	int SubmitId = 0;
	stringstream LogMsg;
	SYSTEMTIME systime;
	
	EnterCriticalSection(&Out);
	CurrentThreadIdx = ThreadsIdx++;
	LeaveCriticalSection(&Out);
	if (!InitThread(CurrentThreadIdx)) return 1;
	if (!InitMySQL(CurrentThreadIdx)) return 1;		
	// ��������� ���� ������ ��������� ������� �������
	while (!Exit) {
		LogMsg.str(string());
		Threads[CurrentThreadIdx].LogMsg = "";
		Threads[CurrentThreadIdx].submit_user_id = 0;
		if (!GetNewSolution(CurrentThreadIdx)) continue;	
		// ���������� � ��� ����� �������� �������
		SubmitId = Threads[CurrentThreadIdx].CurrentSubmit.id;		
		LogMsg << endl << "Solution #" << SubmitId << endl;
		GetLocalTime(&systime);		
		LogMsg << "Testing start time: " << 
			setw(2) << setfill('0') << systime.wDay << "." << 
			setw(2) << setfill('0') << systime.wMonth << "." << 
			systime.wYear << " " <<
			setw(2) << setfill('0') << systime.wHour << ":" << 
			setw(2) << setfill('0') << systime.wMinute << ":" << 
			setw(2) << setfill('0') << systime.wSecond << ":" << 
			setw(3) << setfill('0') << systime.wMilliseconds << 
		endl;
		LogMsg << "Thread id: " << Threads[CurrentThreadIdx].id << endl;
		LogMsg << "User id: " << Threads[CurrentThreadIdx].submit_user_id << endl;
		LogMsg << "Problem id: " << Threads[CurrentThreadIdx].CurrentSubmit.problem_id << endl;

		Threads[CurrentThreadIdx].LogMsg += LogMsg.str();
		LogMsg.str(string());
		// ������� ���� ��� ���������� � ������� ����� ������
		CreateMainFile(CurrentThreadIdx); 
		// ��������� ����������
		if (!PerformCompilation(CurrentThreadIdx)) {
			GetLocalTime(&systime);
			LogMsg << "Testing end time: " << 
				setw(2) << setfill('0') << systime.wDay << "." << 
				setw(2) << setfill('0') << systime.wMonth << "." << 
				systime.wYear << " " <<
				setw(2) << setfill('0') << systime.wHour << ":" << 
				setw(2) << setfill('0') << systime.wMinute << ":" << 
				setw(2) << setfill('0') << systime.wSecond << ":" << 
				setw(3) << setfill('0') << systime.wMilliseconds << 
			endl;
			Threads[CurrentThreadIdx].LogMsg += LogMsg.str();
			LogMsg.str(string());
			LogAndShowProgramEvent(Threads[CurrentThreadIdx].LogMsg);
			continue;
		}
		// ��������� �������
		TestCurrentSolution(CurrentThreadIdx);
		GetLocalTime(&systime);
		LogMsg << "Testing end time: " << 
			setw(2) << setfill('0') << systime.wDay << "." << 
			setw(2) << setfill('0') << systime.wMonth << "." << 
			systime.wYear << " " <<
			setw(2) << setfill('0') << systime.wHour << ":" << 
			setw(2) << setfill('0') << systime.wMinute << ":" << 
			setw(2) << setfill('0') << systime.wSecond << ":" << 
			setw(3) << setfill('0') << systime.wMilliseconds << 
		endl;
		Threads[CurrentThreadIdx].LogMsg += LogMsg.str();
		LogMsg.str(string());
		
		// ������� ��� �������� �������� �������
		LogAndShowProgramEvent(Threads[CurrentThreadIdx].LogMsg);
		// ������� ��� ����� �� ���������� �������� ������
		DelAllFilesInThreadDir(CurrentThreadIdx);
	}
	mysql_close(&Threads[CurrentThreadIdx].mysql);
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// ������� ������ �������� ������ �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD WINAPI WaitThread(PVOID Params) {
	char ExitCommand[MAX_STR_LEN];

	cin.getline(ExitCommand,MAX_STR_LEN);
	while (strcmp(ExitCommand, "Quit") != 0) cin.getline(ExitCommand,MAX_STR_LEN);
	cout << endl << "Terminating threads... Please wait..." << endl;
	Exit = true;
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// ������� ������� ������� �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool RunThreads() {
	stringstream LogMsg;

	Exit = false;
	try {
		for (int i = 0; i < THREAD_COUNT; ++i) {
			hThreads[i] = CreateThread(0, 0, MainThreadsFunction, 0, CREATE_SUSPENDED, NULL);
			if (hThreads[i] == NULL) throw E_CREATE_THREAD;
		}
		hThreads[THREAD_COUNT] = CreateThread(0, 0, WaitThread, 0, CREATE_SUSPENDED, NULL);
		if (hThreads[THREAD_COUNT] == NULL) throw E_CREATE_THREAD;
		for (int i = 0; i <= THREAD_COUNT; ++i) ResumeThread(hThreads[i]);
		WaitForMultipleObjects(THREAD_COUNT + 1, hThreads, true, INFINITE);
		for (int i = 0; i < THREAD_COUNT; ++i) {
			Threads[i].LogMsg = "";
			DelAllFilesInThreadDir(i);
			if (!RemoveDirectory(Threads[i].dir)) 
				LogMsg << "Can't remove thread directory " << Threads[i].id << ": " << GetLastError();
		}
		if (!RemoveDirectory(WorkPath)) 
			LogMsg << "Can't remove thread directory: " << GetLastError();
		throw E_SUCCESSFULL;
	}
	catch (int Exception) {
		LogMsg.str(string());
		if (Exception == E_CREATE_THREAD)
			LogMsg << Messages[Exception].Msg << ": " << GetLastError();
		for (int i = 0; i < THREAD_COUNT; ++i) {
			if (hThreads[i] != NULL) CloseHandle(hThreads[i]);
			LogAndShowProgramEvent(Threads[i].LogMsg);
		}
		if (hThreads[THREAD_COUNT] != NULL) CloseHandle(hThreads[THREAD_COUNT]);
		LogAndShowProgramEvent(LogMsg.str());
		if (Exception == E_SUCCESSFULL) return true;
		return false;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
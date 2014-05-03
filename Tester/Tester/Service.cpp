#include "Service.h"

using namespace std;

ofstream log_file;

// ������� ������ ���������� � ���-����
////////////////////////////////////////////////////////
void LogProgramEvent(char Msg[]) {
	log_file << Msg << endl;
}
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
void LogProgramEvent(char Msg[], int LastError) {
	log_file << Msg << LastError << endl;
}
////////////////////////////////////////////////////////


// ������� ������ ���������� � ���-���� � ������ �� �� �����
////////////////////////////////////////////////////////
void LogAndShowProgramEvent(char Msg[]) {
	log_file << Msg << endl;
	cout << Msg << endl;
}
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
void LogAndShowProgramEvent(char Msg[], int LastError) {
	log_file << Msg << LastError << endl;
	cout << Msg << LastError << endl;
}
////////////////////////////////////////////////////////


// �������� ����� ������� ��� ����������
////////////////////////////////////////////////////////
void CreateMainFile(char solution[], char main_file[], char ThreadDir[]) {
	char fname[MAX_STR_LEN];
	sprintf_s(fname, sizeof(fname), "%s\\%s", ThreadDir, main_file);
	ofstream fout(fname);
	fout << solution;
}
////////////////////////////////////////////////////////


// ������� �������� ���� ������ �� ������� ���������� ������
////////////////////////////////////////////////////////
void DelAllFilesInThreadDir(char ThreadDir[]) {
	char FPath[MAX_STR_LEN], FindFileMask[MAX_STR_LEN];
	HANDLE hFile;
	WIN32_FIND_DATA FileData;

	sprintf_s(FindFileMask, sizeof(FindFileMask), "%s\\*.*", ThreadDir);
	hFile = FindFirstFile(FindFileMask, &FileData);
	if (hFile == INVALID_HANDLE_VALUE) return;
	do {
		if (FileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) continue;
		sprintf_s(FPath, sizeof(FPath), "%s\\%s", ThreadDir, FileData.cFileName);
		if (!DeleteFile(FPath)) LogProgramEvent("Can't delete file: ", GetLastError());
	} while (FindNextFile(hFile, &FileData));
	if (!FindClose(hFile)) LogProgramEvent("Can't close current find operation: ", GetLastError());
}
////////////////////////////////////////////////////////


////////////////////////////////////////////////////////
bool CloseAllForJob(HANDLE job, PROCESS_INFORMATION *ProcessInf) {
	CloseHandle(ProcessInf->hProcess);
	CloseHandle(ProcessInf->hThread);
	if (job == NULL) return true;
	if (!TerminateJobObject(job, 0)) {
		LogAndShowProgramEvent("Error terminating job object: ", GetLastError());
		return false;
	}
	return true;
}
////////////////////////////////////////////////////////

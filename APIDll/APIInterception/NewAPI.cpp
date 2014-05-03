#include "NewAPI.h"

// ������� ��������� ����������
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GenerateException(void) {
	int b = 0, a = 1 / b;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ������� �������� ����� (UNICODE)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HANDLE WINAPI NewCreateFileWFunction(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, 
									 LPSECURITY_ATTRIBUTES lpSecurity, DWORD dwCreationDisp, 
									 DWORD dwFlags, HANDLE hTemplate) {
	if (wcscmp(lpFileName, L"output.txt") != 0 && wcscmp(lpFileName, L"input.txt") != 0) {
		//throw EXCEPTION_ACCESS_VIOLATION;
		GenerateException();
		return NULL;
	}
	return Std_CreateFileW_Function(lpFileName, dwDesiredAccess, dwShareMode, lpSecurity, dwCreationDisp, dwFlags, hTemplate);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ������� �������� ����� (ANSI)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HANDLE WINAPI NewCreateFileAFunction(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, 
									 LPSECURITY_ATTRIBUTES lpSecurity, DWORD dwCreationDisp, 
									 DWORD dwFlags, HANDLE hTemplate) {
	if (strcmp(lpFileName, "output.txt") != 0 && strcmp(lpFileName, "input.txt") != 0) {
		//throw EXCEPTION_ACCESS_VIOLATION;
		GenerateException();
		return NULL;
	}
	return Std_CreateFileA_Function(lpFileName, dwDesiredAccess, dwShareMode, lpSecurity, dwCreationDisp, dwFlags, hTemplate);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ������� �������� �����
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI NewOpenFileFunction(LPCSTR lpFileName, LPOFSTRUCT lpReOpenBuff, UINT uStyle) {
	if (strcmp(lpFileName, "input.txt") != 0 && strcmp(lpFileName, "output.txt") != 0) {
        //throw EXCEPTION_ACCESS_VIOLATION;
		GenerateException();
		return FALSE;
	}
	return Std_OpenFile_Function(lpFileName, lpReOpenBuff, uStyle);   
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ������� �������� ���������� (UNICODE)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI NewCreateDirectoryWFunction(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes) {
	GenerateException();
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ������� �������� ���������� (ANSI)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI NewCreateDirectoryAFunction(LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes) {
	GenerateException();
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ������� �������� ����� (ANSI)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI NewDeleteFileAFunction(LPCSTR lpFileName) {
	GenerateException();
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ������� �������� ����� (UNICODE)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI NewDeleteFileWFunction(LPCWSTR lpFileName) {
	GenerateException();
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ������� �������� ���������� (ANSI)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI NewRemoveDirectoryAFunction(LPCSTR lpPathName) {
	GenerateException();
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ������� �������� ���������� (UNICODE)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI NewRemoveDirectoryWFunction(LPCWSTR lpPathName) {
	GenerateException();
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ������� �������� �������� (ANSI)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI NewCreateProcessAFunction(LPCSTR lpApplicationName, LPCSTR lpCommandLine, 
									  LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, 
									  BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, 
									  LPCSTR lpCurrentDirectory, LPSTARTUPINFO lpStartupInfo, 
									  LPPROCESS_INFORMATION lpProcessInformation) {
	GenerateException();
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ������� �������� �������� (UNICODE)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI NewCreateProcessWFunction(LPCWSTR lpApplicationName, LPCWSTR lpCommandLine, 
									  LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, 
									  BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, 
									  LPCWSTR lpCurrentDirectory, LPSTARTUPINFO lpStartupInfo, 
									  LPPROCESS_INFORMATION lpProcessInformation) {
	GenerateException();
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ������� �������� ������
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HANDLE WINAPI NewCreateThreadFunction(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, 
									  LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, 
									  LPDWORD lpThreadId) {
	GenerateException();
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ������� �������� ���������� ������
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HANDLE WINAPI NewCreateRemoteThreadFunction(HANDLE hProcess, LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, 
											LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, 
											LPDWORD lpThreadId) {
	GenerateException();
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ������� ����������� ������ (ANSI)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI NewCopyFileAFunction(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists) {
	GenerateException();
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ������� ����������� ������ (UNICODE)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI NewCopyFileWFunction(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, BOOL bFailIfExists) {
	GenerateException();
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ������� ����������� ������ (ANSI)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI NewMoveFileAFunction(LPCSTR lpExistingFileName, LPCSTR lpNewFileName) {
	GenerateException();
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ������� ����������� ������ (UNICODE)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI NewMoveFileWFunction(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName) {
	GenerateException();
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ������� ������� ����������
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT WINAPI NewWinExecFunction(LPCSTR lpCmdLine, UINT uCmdShow) {
	GenerateException();
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ������� ���������� ��������
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI NewTerminateProcessFunction(HANDLE hProcess, UINT uExitCode) {
	GenerateException();
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ������� ���������� ������
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI NewTerminateThreadFunction(HANDLE hThread, DWORD dwExitCode) {
	GenerateException();
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ������� ������ ����� (ANSI)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI NewReplaceFileAFunction(LPCSTR lpReplacedFileName, LPCSTR lpReplacementFileName, LPCSTR lpBackupFileName,
									DWORD dwReplaceFlags, LPVOID lpExclude, LPVOID lpReserved) {
	GenerateException();
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ������� ������ ����� (UNICODE)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI NewReplaceFileWFunction(LPCWSTR lpReplacedFileName, LPCWSTR lpReplacementFileName, LPCWSTR lpBackupFileName,
									DWORD dwReplaceFlags, LPVOID lpExclude, LPVOID lpReserved) {
	GenerateException();
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HANDLE WINAPI NewCreateFileMappingAFunction(HANDLE hFile, LPSECURITY_ATTRIBUTES lpAttributes, DWORD flProtect, 
											DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCSTR lpName) {
	GenerateException();
	return NULL;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HANDLE WINAPI NewCreateFileMappingWFunction(HANDLE hFile, LPSECURITY_ATTRIBUTES lpAttributes, DWORD flProtect, 
											DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCWSTR lpName) {
	GenerateException();
	return NULL;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI NewVirtualProtectFunction(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect) {
	GenerateException();
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI NewVirtualProtectExFunction(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, 
										PDWORD lpflOldProtect) {
	GenerateException();
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI NewSetComputerNameAFunction(LPCSTR lpComputerName) {
	GenerateException();
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI NewSetComputerNameWFunction(LPCWSTR lpComputerName) {
	GenerateException();
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						
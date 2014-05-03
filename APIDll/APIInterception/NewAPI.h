#ifndef NEW_API_H
#define NEW_API_H

#include <windows.h>

// Функции для перехвата
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef HANDLE (WINAPI *CreateFileWFunction)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
typedef HANDLE (WINAPI *CreateFileAFunction)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
typedef HFILE (WINAPI *OpenFileFunction)(LPCSTR, LPOFSTRUCT, UINT);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern CreateFileWFunction Std_CreateFileW_Function;
extern CreateFileAFunction Std_CreateFileA_Function;
extern OpenFileFunction Std_OpenFile_Function;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Размер участка функции, в который вставляется код перехвата
#define INTERCEPT_SIZE  5 
// Размер блока памяти для хранения оригинальной функции
#define SAVE_BLOCK_SIZE 10
// Количество переопределяемых функций
#define INTERCEPT_COUNT 28 
// Длина имени функции
#define NAME_LENGTH     30

// Структура - описание перехватываемой функции (имя, адрес новой функции, адрес для сохранения стандартной функции)
typedef struct {
	char FunctionName[NAME_LENGTH];
	void *NewFunction;
	void **StdFunction;
} InterceptDescription;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HANDLE WINAPI NewCreateFileWFunction(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurity, 
							  DWORD dwCreationDisp, DWORD dwFlags, HANDLE hTemplate);
HANDLE WINAPI NewCreateFileAFunction(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurity, 
							  DWORD dwCreationDisp, DWORD dwFlags, HANDLE hTemplate);
BOOL WINAPI NewOpenFileFunction(LPCSTR lpFileName, LPOFSTRUCT lpReOpenBuff, UINT uStyle);
BOOL WINAPI NewCreateDirectoryWFunction(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
BOOL WINAPI NewCreateDirectoryAFunction(LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
BOOL WINAPI NewDeleteFileAFunction(LPCSTR lpFileName);
BOOL WINAPI NewDeleteFileWFunction(LPCWSTR lpFileName);
BOOL WINAPI NewRemoveDirectoryAFunction(LPCSTR lpPathName);
BOOL WINAPI NewRemoveDirectoryWFunction(LPCWSTR lpPathName);

BOOL WINAPI NewCreateProcessAFunction(LPCSTR lpApplicationName, LPCSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, 
									  LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags,
									  LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFO lpStartupInfo, 
									  LPPROCESS_INFORMATION lpProcessInformation);
BOOL WINAPI NewCreateProcessWFunction(LPCWSTR lpApplicationName, LPCWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, 
									  LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags,
									  LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFO lpStartupInfo, 
									  LPPROCESS_INFORMATION lpProcessInformation);
HANDLE WINAPI NewCreateThreadFunction(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, 
									  LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter,
						              DWORD dwCreationFlags, LPDWORD lpThreadId);
HANDLE WINAPI NewCreateRemoteThreadFunction(HANDLE hProcess, LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, 
											LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, 
											LPDWORD lpThreadId);
UINT WINAPI NewWinExecFunction(LPCSTR lpCmdLine, UINT uCmdShow);
BOOL WINAPI NewTerminateProcessFunction(HANDLE hProcess, UINT uExitCode);
BOOL WINAPI NewTerminateThreadFunction(HANDLE hThread, DWORD dwExitCode);

BOOL WINAPI NewCopyFileAFunction(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists);
BOOL WINAPI NewCopyFileWFunction(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, BOOL bFailIfExists);
BOOL WINAPI NewMoveFileAFunction(LPCSTR lpExistingFileName, LPCSTR lpNewFileName);
BOOL WINAPI NewMoveFileWFunction(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName);
BOOL WINAPI NewReplaceFileAFunction(LPCSTR lpReplacedFileName, LPCSTR lpReplacementFileName, LPCSTR lpBackupFileName,
									DWORD dwReplaceFlags, LPVOID lpExclude, LPVOID lpReserved);
BOOL WINAPI NewReplaceFileWFunction(LPCWSTR lpReplacedFileName, LPCWSTR lpReplacementFileName, LPCWSTR lpBackupFileName,
									DWORD dwReplaceFlags, LPVOID lpExclude, LPVOID lpReserved);

HANDLE WINAPI NewCreateFileMappingAFunction(HANDLE hFile, LPSECURITY_ATTRIBUTES lpAttributes, DWORD flProtect, DWORD dwMaximumSizeHigh,
											DWORD dwMaximumSizeLow, LPCSTR lpName);
HANDLE WINAPI NewCreateFileMappingWFunction(HANDLE hFile, LPSECURITY_ATTRIBUTES lpAttributes, DWORD flProtect, DWORD dwMaximumSizeHigh,
											DWORD dwMaximumSizeLow, LPCWSTR lpName);
BOOL WINAPI NewVirtualProtectFunction(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect);
BOOL WINAPI NewVirtualProtectExFunction(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect);

BOOL WINAPI NewSetComputerNameAFunction(LPCSTR lpComputerName);
BOOL WINAPI NewSetComputerNameWFunction(LPCWSTR lpComputerName);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




#endif
#include "NewAPI.h"

// ���������������� �������
/***********************************************/
CreateFileAFunction Std_CreateFileA_Function;
CreateFileWFunction Std_CreateFileW_Function;
OpenFileFunction Std_OpenFile_Function;
/***********************************************/

// �������� ��������������� �������
/*******************************************************************************************************************************************/
InterceptDescription Functions[INTERCEPT_COUNT] = {"CreateFileA",        NewCreateFileAFunction,        (void**)(&Std_CreateFileA_Function),
												   "CreateFileW",        NewCreateFileWFunction,        (void**)(&Std_CreateFileW_Function),
												   "OpenFile",           NewOpenFileFunction,           (void**)(&Std_OpenFile_Function),
												   "CreateDirectoryA",   NewCreateDirectoryAFunction,   NULL,
												   "CreateDirectoryW",   NewCreateDirectoryWFunction,   NULL,
                                                   "DeleteFileA",        NewDeleteFileAFunction,        NULL,
                                                   "DeleteFileW",        NewDeleteFileWFunction,        NULL,
                                                   "RemoveDirectoryA",   NewRemoveDirectoryAFunction,   NULL,
                                                   "RemoveDirectoryW",   NewRemoveDirectoryWFunction,   NULL,
                                                   "CreateProcessA",     NewCreateProcessAFunction,     NULL,
                                                   "CreateProcessW",     NewCreateProcessWFunction,     NULL,
                                                   "CreateThread",       NewCreateThreadFunction,       NULL,
                                                   "CreateRemoteThread", NewCreateRemoteThreadFunction, NULL,
                                                   "CopyFileA",          NewCopyFileAFunction,          NULL,
                                                   "CopyFileW",          NewCopyFileWFunction,          NULL,
                                                   "MoveFileA",          NewMoveFileAFunction,          NULL,
                                                   "MoveFileW",          NewMoveFileWFunction,          NULL,
                                                   "WinExec",            NewWinExecFunction,            NULL,
                                                   "TerminateProcess",   NewTerminateProcessFunction,   NULL,
                                                   "TerminateThread",    NewTerminateThreadFunction,    NULL,
                                                   "ReplaceFileA",       NewReplaceFileAFunction,       NULL,
                                                   "ReplaceFileW",       NewReplaceFileWFunction,       NULL,
                                                   "CreateFileMappingA", NewCreateFileMappingAFunction, NULL,
                                                   "CreateFileMappingW", NewCreateFileMappingWFunction, NULL,
                                                   //"VirtualProtect",     NewVirtualProtectFunction,     NULL,
                                                   //"VirtualProtectEx",   NewVirtualProtectExFunction,   NULL,
                                                   "SetComputerNameA",   NewSetComputerNameAFunction,   NULL,
                                                   "SetComputerNameW",   NewSetComputerNameWFunction,   NULL}; 
/*******************************************************************************************************************************************/

// ��� �������� ���������� jmp_near
const unsigned char JmpOperationCode = 0xE9;

// ���������, ������ ���������� ���������� jmp
#pragma pack(push, 1)
struct Jump {
    BYTE OperationCode; 
    DWORD RelativeAddress;
};
#pragma pack(pop)

// ������� ���������� �������������� ������ ��� ������� jmp
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t CalcRelativeAddressForJmp(void *First, void *Second) {
	return (char*)Second - ((char*)First + INTERCEPT_SIZE);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ������� ������ ����������� ��������������� �������
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CallStdFunction(void *FunctionAddress, void **OriginalFunction) {
	void *MemoryBlock;
	size_t OriginalRelativeAddress;

	MemoryBlock = VirtualAlloc(NULL, SAVE_BLOCK_SIZE, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	OriginalRelativeAddress = CalcRelativeAddressForJmp(MemoryBlock, FunctionAddress); 
	// ��������� ������ 5 ���� ����������� �������,
	memcpy_s((char*)MemoryBlock, INTERCEPT_SIZE, FunctionAddress, INTERCEPT_SIZE);
	// ��� �������� ��� jmp
	memcpy_s((char*)MemoryBlock + INTERCEPT_SIZE, sizeof(JmpOperationCode), &JmpOperationCode, sizeof(JmpOperationCode));
	// � ����� ��� �������� �� ������������ �������
	memcpy_s((char*)MemoryBlock + INTERCEPT_SIZE + sizeof(JmpOperationCode), sizeof(size_t), &OriginalRelativeAddress, sizeof(size_t));
	*OriginalFunction = MemoryBlock;
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ������� ���������
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CallInterception(char FunctionName[], void *NewFunctionAddress, void **OriginalFunction) {
	DWORD dwProtect = PAGE_READWRITE;
	Jump* FunctionAddress;
	HMODULE hKernel32;

	hKernel32 = GetModuleHandle("kernel32.dll");
    FunctionAddress = (Jump*)(GetProcAddress(hKernel32, FunctionName));
	if (OriginalFunction != NULL) CallStdFunction(FunctionAddress, OriginalFunction);
	VirtualProtect(FunctionAddress, sizeof(Jump), dwProtect, &dwProtect);
	FunctionAddress->OperationCode = JmpOperationCode;
	FunctionAddress->RelativeAddress = CalcRelativeAddressForJmp(FunctionAddress, NewFunctionAddress);
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// �������� dll-�������
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
		for (int i = 0; i < 26; ++i)
			CallInterception(Functions[i].FunctionName, Functions[i].NewFunction, Functions[i].StdFunction);		
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
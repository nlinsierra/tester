#ifndef SERVICE_H
#define SERVICE_H

#include "Global.h"

void LogProgramEvent(char Msg[]);
void LogProgramEvent(char Msg[], int LastError);
void LogAndShowProgramEvent(char Msg[]);
void LogAndShowProgramEvent(char Msg[], int LastError);

void CreateMainFile(char solution[], char main_file[], char ThreadDir[]);
void DelAllFilesInThreadDir(char ThreadDir[]);
bool CloseAllForJob(HANDLE job, PROCESS_INFORMATION *ProcessInf);
DWORD GetProcessMem(HANDLE hProcess);

#endif
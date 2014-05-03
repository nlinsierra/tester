#include "LimitSettings.h"

using namespace std;

// Функция настройки ограничений для объекта Job
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SetBaseLimits(HANDLE job, int MaxTime, int MaxMem, int CurrentThreadIdx) {
	JOBOBJECT_BASIC_LIMIT_INFORMATION JobInf;
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION ExJobInf;
	stringstream LogMsg;

	JobInf.LimitFlags = JOB_OBJECT_LIMIT_PROCESS_TIME | 
		                JOB_OBJECT_LIMIT_PROCESS_MEMORY | 
						JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION | 
						JOB_OBJECT_LIMIT_ACTIVE_PROCESS |
						JOB_OBJECT_LIMIT_JOB_MEMORY |
						JOB_OBJECT_LIMIT_JOB_TIME |
						JOB_OBJECT_LIMIT_PRIORITY_CLASS;
	JobInf.PerProcessUserTimeLimit.QuadPart = MaxTime * NANOSECS;	
	JobInf.ActiveProcessLimit = 1;
	JobInf.PriorityClass = IDLE_PRIORITY_CLASS;
	JobInf.PerJobUserTimeLimit.QuadPart = MaxTime * NANOSECS;

	ExJobInf.BasicLimitInformation = JobInf;
	ExJobInf.ProcessMemoryLimit = MaxMem;
	ExJobInf.JobMemoryLimit = MaxMem;
	try {
		if (!SetInformationJobObject(job, JobObjectExtendedLimitInformation, &ExJobInf, sizeof(ExJobInf))) throw E_SET_JOB_LIMITS;
		return true;
	}
	catch (int Exception) {
		LogMsg << Messages[Exception - 1].Msg << ": " << GetLastError() << endl;
		Threads[CurrentThreadIdx].LogMsg += LogMsg.str();
		return false;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Функция настройки дополнительной информации для объекта Job
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SetAdditionalInformation(HANDLE job, int CurrentThreadIdx) {
	JOBOBJECT_END_OF_JOB_TIME_INFORMATION EndOfJobTimeInf;
	JOBOBJECT_BASIC_UI_RESTRICTIONS BasicRestrictions;
	stringstream LogMsg;

	try {
		EndOfJobTimeInf.EndOfJobTimeAction = JOB_OBJECT_POST_AT_END_OF_JOB;
		if (!SetInformationJobObject(job, JobObjectEndOfJobTimeInformation, &EndOfJobTimeInf, sizeof(EndOfJobTimeInf))) 
			throw E_SET_JOB_TIME_INFORMATION;
		BasicRestrictions.UIRestrictionsClass = JOB_OBJECT_UILIMIT_ALL;
		if (!SetInformationJobObject(job, JobObjectBasicUIRestrictions, &BasicRestrictions, sizeof(BasicRestrictions))) 
			throw E_SET_JOB_RESTRICTIONS;
		return true;
	}
	catch (int Exception) {
		LogMsg << Messages[Exception - 1].Msg << ": " << GetLastError() << endl;
		Threads[CurrentThreadIdx].LogMsg += LogMsg.str();
		return false;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Общая функция настройки ограничений
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SetLimits(HANDLE job, int MaxTime, int MaxMem, int CurrentThreadIdx) {
	return SetBaseLimits(job, MaxTime, MaxMem, CurrentThreadIdx) && SetAdditionalInformation(job, CurrentThreadIdx);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
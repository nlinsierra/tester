#include "Global.h"

using namespace std;

// Функция записи информации в лог-файл и вывода ее на экран
/////////////////////////////////////////////////////////////
void LogAndShowProgramEvent(string Msg) {
	EnterCriticalSection(&Out);
	log_file << Msg;
	cout << Msg;
	LeaveCriticalSection(&Out);
}
/////////////////////////////////////////////////////////////

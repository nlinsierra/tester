#include "Global.h"

using namespace std;

// ������� ������ ���������� � ���-���� � ������ �� �� �����
/////////////////////////////////////////////////////////////
void LogAndShowProgramEvent(string Msg) {
	EnterCriticalSection(&Out);
	log_file << Msg;
	cout << Msg;
	LeaveCriticalSection(&Out);
}
/////////////////////////////////////////////////////////////

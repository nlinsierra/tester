#include "Run.h"

using namespace std;

CRITICAL_SECTION Out;
ofstream log_file;
char LogFileName[MAX_STR_LEN], CurrentDate[MAX_STR_LEN], CurrentTime[MAX_STR_LEN];
SYSTEMTIME systime;

int main() {
	try {
		GetLocalTime(&systime);
		sprintf_s(CurrentDate, sizeof(CurrentDate), "%d_%d_%d", systime.wDay, systime.wMonth, systime.wYear);
		sprintf_s(CurrentTime, sizeof(CurrentTime), "%d_%d_%d", systime.wHour, systime.wMinute, systime.wSecond);
		sprintf_s(LogFileName, sizeof(LogFileName),"TesterLog_%s_%s.txt", CurrentDate, CurrentTime);
		InitializeCriticalSection(&Out);
		if (!InitTester()) throw E_INIT_TESTER;
		if (!PasswordRequest()) throw E_PASSWORD_REQUEST;
		log_file.open(LogFileName);
		RunThreads();
	}
	catch (...) {
		cout << "Initialization error." << endl << endl;
		log_file.close();
	}
	return 0;
}
#include <fstream>
#include <iostream>
using namespace std;

#define WRONG_ANSWER		-1
#define ACCEPTED			0
#define PRESENTATION_ERROR	1

#define STR_SIZE			10000

void Trim(char str[]);

extern "C" __declspec(dllexport) int CompareFiles(char ProgramOutput[], char CorrectOutput[]) {
	ifstream fp(ProgramOutput), fc(CorrectOutput);
	char cc, cc_str[STR_SIZE], cp;
	int i = 0;
	while (true) {
		cc = fc.get();
		if (fc.eof()) break;
		cc_str[i++] = cc;
	}
	cc_str[i] = '\0';
	Trim(cc_str);
	for (i = 0; i < strlen(cc_str); ++i) {
		cp = fp.get();
		cout << cp;
		if (cp != cc_str[i])
			return WRONG_ANSWER;
	}
	cout << endl;
	return ACCEPTED;
}

void Trim(char str[]) {
	for (int i = strlen(str) - 1; i >= 0; i--) {
		if (str[i] == '\n' || str[i] == '\r' || str[i] == ' ')
			str[i] = '\0';
		else return;
	}
}
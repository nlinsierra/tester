#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

#define WRONG_ANSWER		-1
#define ACCEPTED			0
#define PRESENTATION_ERROR	1

#define STR_SIZE			10000000
#define MAX_SIZE			1000010

//char cc, cc_str[STR_SIZE], cp;
vector<int> result, prog_result;

void Trim(char str[]);

extern "C" __declspec(dllexport) int CompareFiles(char ProgramOutput[], char CorrectOutput[]) {
	ifstream fp(ProgramOutput), fc(CorrectOutput);
	
	/*int i = 0;
	while (true) {
		cc = fc.get();
		if (fc.eof()) break;
		cc_str[i++] = cc;
	}
	cc_str[i] = '\0';
	Trim(cc_str);
	for (i = 0; i < strlen(cc_str); ++i) {
		cp = fp.get();
		if (cp != cc_str[i])
			return WRONG_ANSWER;
	}
	return ACCEPTED;*/
	int i = 0;
	while (true) {
		int tmp;
		fc >> tmp;
		if (fc.eof()) break;
		result.push_back(tmp);
	}
	i = 0;
	while (true) {
		++i;
		int tmp;
		fp >> tmp;
		if (fp.eof()) break;
		prog_result.push_back(tmp);
		if (i >= MAX_SIZE) break;
	}
	if (result.size() != prog_result.size()) return WRONG_ANSWER;
	for (i = 0; i < result.size(); ++i) {		
		if (prog_result[i] != result[i]) return WRONG_ANSWER;
	}	
	return ACCEPTED;
}

void Trim(char str[]) {
	for (int i = strlen(str) - 1; i >= 0; i--) {
		if (str[i] == '\n' || str[i] == '\r' || str[i] == ' ')
			str[i] = '\0';
		else return;
	}
}
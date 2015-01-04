#ifndef GLOBAL_H
#define GLOBAL_H

#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>

#include <stdlib.h>
#include <winsock.h>
#include <process.h>
#include <Processthreadsapi.h>
#include <psapi.h>
#include <time.h>
#include <io.h>
#include <conio.h>

#include "mysql.h"

using namespace std;

// ������������ ����� ������ � ���������
#define MAX_STR_LEN 500

// ���������� ������������ ���������� �������
#define THREAD_COUNT 1

#define TRY_COUNT 5

// ��������� �������� ����� �������� ����
#define DELAY 3000

// �������� �������� ���������� �������� �������
#define WAIT_INTERVAL 30000

// �������� �������� ������� ���������� ���������-�������
#define TIME_CHECK_INTERVAL 10

// ������������ ������ ��������� ���� �������
#define SOLUTION_SIZE 1048576

// ������������ ����� ��������� ����
#define LOG_MSG_LEN 100000

// ��������� ��� ����������� ������� ���������� ��������� � ������������ �� ������
#define NANOSECS 10000
#define KB       1024

// ��������� ��� ����������� ��������� ������� ���������� �������
#define FAILED_RESULT -1

// �������������� ���������
/****************************/
#define RUNTIME_ERROR		0
#define COMPILE_ERROR		1
#define TIME_LIMIT			2
#define MEMORY_LIMIT		3
#define WRONG_ANSWER		4
#define ACCEPTED			5
#define INTERNAL_ERROR		6
#define PRESENTATION_ERROR	7
/****************************/

// ���������� ����������
#define EXCEPTION_COUNT 40

// �������������� ����������
/*******************************************/
#define E_COMPLETION_PORT_ASSOCIATION  1
#define E_READ_JOB_MEMORY_INFORMATION  2
#define E_TERMINATE_JOB_OBJECT         3
#define E_CREATE_JOB_OBJECT            4
#define E_CREATE_COMPLETION_PORT       5
#define E_SET_LIMITS                   6
#define E_ASSIGN_PROCESS_TO_JOB        7
#define E_CLOSE_ALL_FOR_JOB            8
#define E_MEMORY_LIMIT                 9
#define E_RESUME_THREAD               10
#define E_READ_COMPLETION_PORT        11
#define E_CREATE_PROCESS              12
#define E_SUCCESSFULL                 13
#define E_COMPILE_ERROR               14
#define E_SET_JOB_INFORMATION         15
#define E_NO_TESTS                    16
#define E_COPY_INPUT_FILE             17
#define E_LIBRARY_ACCESS              18
#define E_READ_DLL_FUNCTION           19
#define E_CREATE_PIPE                 20
#define E_CREATE_COMPILATION_PROCESS  21
#define E_SET_JOB_LIMITS              22
#define E_SET_JOB_RESTRICTIONS        23
#define E_SET_JOB_TIME_INFORMATION    24
#define E_LOCK_TABLE_SQL              25
#define E_SELECT_SOLUTION_SQL         26
#define E_NO_SOLUTION                 27
#define E_UPDATE_CHECK_STATE          28
#define E_CREATE_THREAD               29
#define E_INIT_TESTER                 30
#define E_PASSWORD_REQUEST            31
#define E_INTERSEPT_API               32
#define E_TIME_LIMIT                  33
#define E_RUNTIME_ERROR				  34
/*******************************************/

// ��������� - ��������� ��� ����������
typedef struct {
	char Msg[MAX_STR_LEN];
} ExceptionMessage;

// ��������� - ������ ��������� ��� ��������� ���� ����������
/*****************************************************************************************************/
const ExceptionMessage Messages[EXCEPTION_COUNT] = {"Error associating completion port with job",
                                                    "Error reading job memory information",
                                                    "Error terminating job object",
                                                    "Error creating job object",
                                                    "Error creating completion port",
                                                    "",
                                                    "Error assigning process to job object",
                                                    "",
                                                    "",
                                                    "Error resuming thread",
                                                    "Error reading completion port informaition",
                                                    "Creating process error",
                                                    "",
                                                    "",
                                                    "",
                                                    "",
                                                    "Error coping input file",
                                                    "Library access error",
                                                    "Error reading dll function",
                                                    "Creating pipe error",
                                                    "Creating compilation process error",
                                                    "Error setting limit information for job",
                                                    "Error setting job basic restrictions",
                                                    "Error setting end of job time information",
                                                    "Lock table SQL query error",
                                                    "Select solution SQL query error",
                                                    "",
                                                    "Update check state SQL query error",
                                                    "Can't create working thread",
                                                    "",
                                                    "",
                                                    "Interception API error",
                                                    ""};
/*****************************************************************************************************/

// ��������� - ���������� �� ���� �������
typedef struct {
	int id;
	int problem_id;
	int compiler_id;
	char main_file[MAX_STR_LEN];
	char solution[SOLUTION_SIZE];
	int check_result;
	LONGLONG time_used;
	SIZE_T memory_used;
} Submits;

// ��������� - ��������� ������������ �������
typedef struct {
	int VerdictId;
	int TestNum;
	string TestIn;
	string ProgramOut;
	string CorrectOut;
} TestResult;

// ��������� - ���������� � ������
typedef struct {
	int id;
	char dir[MAX_STR_LEN];
	//char LogMsg[LOG_MSG_LEN];
	string LogMsg;
	MYSQL mysql;
	Submits CurrentSubmit;
	int submit_user_id;
} ThreadsInformaition;

extern ThreadsInformaition Threads[THREAD_COUNT];

// ��������� - �������� ������
typedef struct {
	char Id[MAX_STR_LEN];
	char Checker[MAX_STR_LEN];
	int MaxMem;
	int MaxTime;
} ProblemDef;

// ��������� - �������� �����������
typedef struct {
	char Id[MAX_STR_LEN];
	char MainFile[MAX_STR_LEN];
	char CommandFmt[MAX_STR_LEN];
} CompilerDef;

// ��������� - ��������� ��������
typedef struct {
	char content[MAX_STR_LEN];
} Verdict;

// ������� ��������� �������, ������������ � dll
typedef bool (*CompareFunction) (char[], char[]);

// ������ - �������� �����
extern std::vector<ProblemDef> Problems;
// ������ - �������� ������������
extern std::vector<CompilerDef> Compilers;
// ������ - �������� ���������
extern std::vector<Verdict> Verdicts;

// ��������� ���� ������
/*************************************/
// ��� ������� ��
extern char Servername[MAX_STR_LEN];
// ��� ������������
extern char Username[MAX_STR_LEN];
// ������
extern char Password[MAX_STR_LEN];
// �������� ��
extern char DBName[MAX_STR_LEN];
/*************************************/

// ���� � ���������� � ��������
extern char CurDir[MAX_STR_LEN];

// ���� � ������� ����������
extern char WorkPath[MAX_STR_LEN];

// ��� ��������� exe-�����
const char ExeName[MAX_STR_LEN] = "main.exe";

// ���� � ����� � �������
extern char TestsPath[MAX_STR_LEN];

// ���-����
extern std::ofstream log_file;

// ����������� ������ ��� ����������� ������ ��������� �������
extern CRITICAL_SECTION Out;

// ������� ������ ��������� � ���, ������������� � WriteLog.cpp
void LogAndShowProgramEvent(string Msg);

// ������� �������������, ������������� � Init.cpp
/**************************************************/
bool InitTester();
bool InitThread(int CurrentThreadIdx);
bool InitMySQL(int CurrentThreadIdx);
/**************************************************/

// ������� ������� ������ ���� ������, ������������� � Init.cpp
bool PasswordRequest();

#endif
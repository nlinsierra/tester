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

// Максимальная длина строки в программе
#define MAX_STR_LEN 500

// Количество одновременно запущенных потоков
#define THREAD_COUNT 1

#define TRY_COUNT 5

// Временная задержка между опросами базы
#define DELAY 3000

// Интервал ожидания завершения процесса решения
#define WAIT_INTERVAL 30000

// Интервал подсчета времени выполнения программы-решения
#define TIME_CHECK_INTERVAL 10

// Максимальный размер исходного кода решения
#define SOLUTION_SIZE 1048576

// Максимальная длина сообщения лога
#define LOG_MSG_LEN 100000

// Константы для определения времени выполнения программы и потребляемой ею памяти
#define NANOSECS 10000
#define KB       1024

// Константа для обозначения неудачных попыток выполнения функция
#define FAILED_RESULT -1

// Идентификаторы вердиктов
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

// Количество исключений
#define EXCEPTION_COUNT 40

// Идентификаторы исключений
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

// Структура - сообщение при исключении
typedef struct {
	char Msg[MAX_STR_LEN];
} ExceptionMessage;

// Константа - массив сообщений для описанных выше исключений
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

// Структура - полученное из базы решение
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

// Структура - результат тестирования решения
typedef struct {
	int VerdictId;
	int TestNum;
	string TestIn;
	string ProgramOut;
	string CorrectOut;
} TestResult;

// Структура - информация о потоке
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

// Структура - описание задачи
typedef struct {
	char Id[MAX_STR_LEN];
	char Checker[MAX_STR_LEN];
	int MaxMem;
	int MaxTime;
} ProblemDef;

// Структура - описание компилятора
typedef struct {
	char Id[MAX_STR_LEN];
	char MainFile[MAX_STR_LEN];
	char CommandFmt[MAX_STR_LEN];
} CompilerDef;

// Структура - сообщение вердикта
typedef struct {
	char content[MAX_STR_LEN];
} Verdict;

// Функция сравнения решений, определенная в dll
typedef bool (*CompareFunction) (char[], char[]);

// Вектор - описание задач
extern std::vector<ProblemDef> Problems;
// Вектор - описание компиляторов
extern std::vector<CompilerDef> Compilers;
// Вектор - описание вердиктов
extern std::vector<Verdict> Verdicts;

// Настройки базы данных
/*************************************/
// Имя сервера БД
extern char Servername[MAX_STR_LEN];
// Имя пользователя
extern char Username[MAX_STR_LEN];
// Пароль
extern char Password[MAX_STR_LEN];
// Название БД
extern char DBName[MAX_STR_LEN];
/*************************************/

// Путь к директории с тестером
extern char CurDir[MAX_STR_LEN];

// Путь к рабочей директории
extern char WorkPath[MAX_STR_LEN];

// Имя основного exe-файла
const char ExeName[MAX_STR_LEN] = "main.exe";

// Путь к папке с тестами
extern char TestsPath[MAX_STR_LEN];

// Лог-файл
extern std::ofstream log_file;

// Критическая секция для синхронного вывода сообщений потоков
extern CRITICAL_SECTION Out;

// Функция записи сообщений в лог, реализованная в WriteLog.cpp
void LogAndShowProgramEvent(string Msg);

// Функции инициализации, реализованные в Init.cpp
/**************************************************/
bool InitTester();
bool InitThread(int CurrentThreadIdx);
bool InitMySQL(int CurrentThreadIdx);
/**************************************************/

// Функция запроса пароля базы данных, реализованная в Init.cpp
bool PasswordRequest();

#endif
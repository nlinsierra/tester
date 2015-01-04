#include "Global.h"
using namespace std;

/*
PROJECT menu->PROPERTIES.
Under C++->GENERAL , add c:\mysql\include.
Under LINKER->GENERAL, add c:\mysql\lib\opt. 
PROJECT menu->PROPERTIES -> LINKER->INPUT -> ADDITIONAL DEPENDENCIES -> add libmysql.lib. 
*/

// Данные, считываемые из файла config.ini
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Имя сервера БД
char Servername[MAX_STR_LEN];

// Имя пользователя для подключения к БД
char Username[MAX_STR_LEN];

// Пароль пользователя для подключения к БД
char Password[MAX_STR_LEN];

// Имя базы данных
char DBName[MAX_STR_LEN];

// Описание задач текущего турнира (буквенный идентификатор и имя чекера)
vector<ProblemDef> Problems;

// Описание доступных компиляторов
vector<CompilerDef> Compilers;

// Описание вердиктов тестирования
vector<Verdict> Verdicts;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Путь к директории с тестером
char CurDir[MAX_STR_LEN];

// Путь к рабочей директории
char WorkPath[MAX_STR_LEN];

// Путь к директории с тестами
char TestsPath[MAX_STR_LEN];

// Функция чтения информации о задачах из config.ini
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ReadProblemsInf(MYSQL *mysql) {
	char query[MAX_STR_LEN];
	MYSQL_RES *sql_res;
	MYSQL_ROW row;
	ProblemDef CurrentProblem;

	sprintf_s(query, sizeof(query), "select title, max_mem, max_time, checker from problems");
	if (mysql_query(mysql, query) != 0) return false;
	sql_res = mysql_store_result(mysql);	
	Problems.resize(0);
	for (int i = 0; i < sql_res->row_count; ++i) {
		row = mysql_fetch_row(sql_res);
		strcpy_s(CurrentProblem.Id, row[0]);
		CurrentProblem.MaxMem = atoi(row[1]);
		CurrentProblem.MaxTime = atoi(row[2]);
		strcpy_s(CurrentProblem.Checker, row[3]);
		Problems.push_back(CurrentProblem);
	}
	mysql_free_result(sql_res);
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Функция чтения информации о компиляторах из config.ini
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ReadCompilersInf(MYSQL *mysql) {
	char query[MAX_STR_LEN];
	MYSQL_RES *sql_res;
	MYSQL_ROW row;
	CompilerDef CurrentCompiler;

	sprintf_s(query, sizeof(query), "select title, mainfile, command_fmt from compilers");
	if (mysql_query(mysql, query) != 0) return false;
	sql_res = mysql_store_result(mysql);
	Compilers.resize(0);
	for (int i = 0; i < sql_res->row_count; ++i) {
		row = mysql_fetch_row(sql_res);
		strcpy_s(CurrentCompiler.Id, row[0]);
		strcpy_s(CurrentCompiler.MainFile, row[1]);
		strcpy_s(CurrentCompiler.CommandFmt, row[2]);
		Compilers.push_back(CurrentCompiler);
	}
	mysql_free_result(sql_res);
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Функция чтения информации о вердиктах
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ReadVerdicts(MYSQL *mysql) {
	char query[MAX_STR_LEN];
	MYSQL_RES *sql_res;
	MYSQL_ROW row;
	Verdict CurrentVerdict;

	sprintf_s(query, sizeof(query), "select content from verdicts");
	if (mysql_query(mysql, query) != 0) return false;
	sql_res = mysql_store_result(mysql);	
	Verdicts.resize(0);
	for (int i = 0; i < sql_res->row_count; ++i) {
		row = mysql_fetch_row(sql_res);
		strcpy_s(CurrentVerdict.content, strlen(row[0]) + 1, row[0]);
		Verdicts.push_back(CurrentVerdict);
	}
	mysql_free_result(sql_res);
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Функция чтения параметров из config.ini
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ReadConfig() {
	GetPrivateProfileString("Database description", "Servername", "-1", Servername, 
		                    sizeof(Servername), "./config.ini");
	if (strcmp(Servername, "-1") == 0) return false;
	GetPrivateProfileString("Database description", "Username", "-1", Username, 
		                    sizeof(Username), "./config.ini");
	if (strcmp(Username, "-1") == 0) return false;
	GetPrivateProfileString("Database description", "DatabaseName", "-1", DBName, 
		                    sizeof(DBName), "./config.ini");
	if (strcmp(DBName, "-1") == 0) return false;

	GetPrivateProfileString("System description", "TestsPath", "-1", TestsPath, 
		                    sizeof(TestsPath), "./config.ini");
	if (strcmp(DBName, "-1") == 0) return false;

	cout << "Reading configuration file is succesfull" << endl << endl;
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Инициализация тестера
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool InitTester() {
	if (!ReadConfig()) {
		cout << "Error reading configuration file" << endl << endl;
		return false;
	}
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Функция инициализации потока
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool InitThread(int CurrentThreadIdx) {
	stringstream LogMsg;
	// Сохраняем идентификатор потока для доступа к рабочей папке потока
	Threads[CurrentThreadIdx].id = GetCurrentThreadId();
	sprintf_s(Threads[CurrentThreadIdx].dir, sizeof(Threads[CurrentThreadIdx].dir), "%s\\%d", WorkPath, Threads[CurrentThreadIdx].id);
	// Создаем рабочую директорию текущего потока
	if (!CreateDirectory(Threads[CurrentThreadIdx].dir, NULL)) {
		LogMsg << "Error creating working directory: " << GetLastError() << endl;
		Threads[CurrentThreadIdx].LogMsg += LogMsg.str();
		LogAndShowProgramEvent(Threads[CurrentThreadIdx].LogMsg);
		return false;
	}
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Функция инициализации соединения с базой данных
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool InitMySQL(int CurrentThreadIdx) {
	// Соединяемся с базой данных
	if (mysql_init(&Threads[CurrentThreadIdx].mysql) == NULL) {
		Threads[CurrentThreadIdx].LogMsg = "Can't init MySQL connection\n";
		LogAndShowProgramEvent(Threads[CurrentThreadIdx].LogMsg);
		return false;
	}
	mysql_options(&Threads[CurrentThreadIdx].mysql, MYSQL_READ_DEFAULT_GROUP, "Tester");
	if (!mysql_real_connect(&Threads[CurrentThreadIdx].mysql, Servername, Username, Password, DBName, 0, NULL, 0)) {
		Threads[CurrentThreadIdx].LogMsg = string(mysql_error(&Threads[CurrentThreadIdx].mysql)) + "\n";
		LogAndShowProgramEvent(Threads[CurrentThreadIdx].LogMsg);
		return false;
	}
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Функция скрытого ввода пароля
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HideInputPassword() {
	while (true) {
		switch(char c = _getch()) {
		case '\r':
			return;
		case '\b':
			Password[strlen(Password) - 1] = '\0';
			cout << "\b \b";
			break;
		default: 
			sprintf_s(Password, sizeof(Password), "%s%c", Password, c);
			cout << '*';
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Функция запроса пароля на подключение к базе
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool PasswordRequest() {
	MYSQL mysql;
	int TryCount = 0;
	
	while (TryCount < TRY_COUNT) {
		cout << "Please, enter database password: " << endl;
		sprintf_s(Password, sizeof(Password), "");
		HideInputPassword();
		cout << endl;
		if (mysql_init(&mysql) == NULL) {
			cout << "Can't init test MySQL connection" << endl;
			return false;
		}
		mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "Tester");			
		if (!mysql_real_connect(&mysql, Servername, Username, Password, DBName, 0, NULL, 0)) {
			cout << mysql_error(&mysql) << endl << endl;
			if (TryCount == TRY_COUNT - 1) return false;
			cout << "Do you want to try again? (Y/N)" << endl;
			fflush(stdin);
			if (getchar() != 'Y') return false;
			cout << endl;
			++TryCount;
			continue;
		}
		if (!ReadProblemsInf(&mysql)) {
			cout << "Error reading problems information." << endl;
			return false;
		}
		if (!ReadVerdicts(&mysql)) {
			cout << "Error reading verdicts information." << endl;
			return false;
		}
		if (!ReadCompilersInf(&mysql)) {
			cout << "Error reading compilers information." << endl;
			return false;
		}
		break;
	}
	cout << "Reading mysql data is succesfull." << endl << endl;
	system("cls");
	mysql_close(&mysql);	
	GetCurrentDirectory(MAX_STR_LEN,CurDir);
	sprintf_s(WorkPath, sizeof(WorkPath), "%s\\Workdir", CurDir);
	CreateDirectory(WorkPath, NULL);
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////







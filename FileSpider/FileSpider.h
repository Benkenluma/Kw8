#include <windows.h>
#include <iostream>
#include <time.h>
using namespace std;

#include "..\sqlite3\sqlite3.h"

#ifdef _DEBUG
#   pragma comment(lib, "..\\bin\\sqlite3_d.lib")
#else
#   pragma comment(lib, "..\\bin\\sqlite3.lib")
#endif







class FileSpider{
public:
	FileSpider();
	~FileSpider();

	void StartSpider(string);
private:
	void SQL_insert(string, int, __int64, int);
	void BeginSQL();
	void CommitSQL();
	int EnumFileAndFolder(string , int);
private:
	sqlite3 *db;
	sqlite3_stmt *stmt;
	int fileIndex;
};
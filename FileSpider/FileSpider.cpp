#include "FileSpider.h"


FileSpider::FileSpider(){
	//用于创建数据库
	time_t t = time(0);
	char szBuf[16] = { 0 };
	sprintf_s(szBuf, sizeof(szBuf), "%d.db", t);
	int rc = sqlite3_open(szBuf, &db);
	if (!rc){
		string sql = "CREATE TABLE items("  \
			"id				INTEGER PRIMARY KEY     NOT NULL," \
			"parent_id      INTEGER," \
			"name           VARCHAR," \
			"size			NUMERIC," \
			"type			CHAR(1) );";
		rc = sqlite3_exec(db, sql.c_str(), 0, 0, 0);
	}
	//关闭写同步提高效率
	sqlite3_exec(db, "PRAGMA synchronous = OFF; ", 0, 0, 0);

	//文件索引归1
	fileIndex = 1;
}

FileSpider::~FileSpider(){
	sqlite3_close(db);
}

void  FileSpider::SQL_insert(string name, int parent_id, __int64 size, int type){
	//使用执行准备提高效率
	sqlite3_reset(stmt);
	sqlite3_bind_int(stmt, 1, fileIndex);
	sqlite3_bind_int(stmt, 2, parent_id);
	sqlite3_bind_text(stmt, 3, name.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_int64(stmt, 4, size);
	sqlite3_bind_int(stmt, 5, type);
	sqlite3_step(stmt);
}

void FileSpider::BeginSQL(){
	//显式开启事务提高效率
	sqlite3_exec(db, "begin;", 0, 0, 0);
	string sql = "INSERT INTO items (id,parent_id,name,size,type) VALUES (?,?,?,?,?);";
	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, 0);
}

void FileSpider::CommitSQL(){
	sqlite3_finalize(stmt);
	sqlite3_exec(db, "commit;", 0, 0, 0);
}

int FileSpider::EnumFileAndFolder(string path, int parent_id){
	string szFind;
	WIN32_FIND_DATA FindFileData;
	szFind = path + "\\*.*";
	HANDLE hFind = FindFirstFile(szFind.c_str(), &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)
		return -1;
	do{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
			if (strcmp(FindFileData.cFileName, ".") != 0 && strcmp(FindFileData.cFileName, "..") != 0){
				//发现子目录，递归
				string szFile;
				szFile = path + "\\";
				szFile += FindFileData.cFileName;
				SQL_insert(FindFileData.cFileName, parent_id, 0, 1);
				fileIndex++;
				EnumFileAndFolder(szFile , fileIndex - 1);
			}
		}else{
			//找到文件
			SQL_insert(FindFileData.cFileName, parent_id, ((__int64)FindFileData.nFileSizeHigh << 32) + FindFileData.nFileSizeLow, 2);
			fileIndex++;
		}
	} while (FindNextFile(hFind, &FindFileData));

	FindClose(hFind);

	return 0;
}

void FileSpider::StartSpider(string path){
	
	BeginSQL();
	EnumFileAndFolder(path , 0);
	CommitSQL();
}

void getTime(){
	time_t t = time(0);
	char szBuf[13] = { 0 };
	sprintf_s(szBuf, sizeof(szBuf), "%d", t);
	cout << szBuf << endl;
}

void main()
{
	FileSpider fs;
	getTime();
	fs.StartSpider("C:");
	getTime();
	system("pause");
}
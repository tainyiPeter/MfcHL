#include "pch.h"
#include "CSqliteHelp.h"

CSqliteHelp::CSqliteHelp()
{
}

CSqliteHelp::~CSqliteHelp()
{
}

int32_t CSqliteHelp::Open(const std::string& strDbName)
{
	int32_t rc = sqlite3_open("test.db", &db_handle);
	if (rc != SQLITE_OK) 
	{
		//std::cerr << "无法打开数据库: " << sqlite3_errmsg(db) << std::endl;
		return rc;
	}
	return SQLITE_OK;
}

int32_t CSqliteHelp::ExeSql(const std::string& strSql)
{
	char* errMsg = nullptr;
	int32_t rc = sqlite3_exec(db_handle, strSql.c_str(), 0, 0, &errMsg);
	if (rc != SQLITE_OK) {
		//std::cerr << "SQL 错误: " << errMsg << std::endl;
		sqlite3_free(errMsg);
		return rc;
	}
	return SQLITE_OK;
}

void CSqliteHelp::Close()
{
	sqlite3_close(db_handle);
	db_handle = nullptr;
}

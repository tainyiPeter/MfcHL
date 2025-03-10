#pragma once
#include <string>
#include "Sqlite/sqlite3.h"

class CSqliteHelp
{
public:
	CSqliteHelp();
	~CSqliteHelp();
	int32_t Open(const std::string& strDbName);
	int32_t ExeSql(const std::string& strSql);
	void Close();
private:
	sqlite3* db_handle = nullptr;
};
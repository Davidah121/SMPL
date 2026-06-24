#include"ext/SqliteDatabase.h"
#include "sqlite3.h"
#include <string>
#ifdef USE_SQLITE

SqliteStatement::SqliteStatement(sqlite3_stmt* stmt)
{
	this->stmt = stmt;
}

SqliteStatement::~SqliteStatement()
{
	if(stmt != nullptr)
		sqlite3_finalize(stmt);
	stmt = nullptr;
}

void SqliteStatement::bindBlob(int index, const void* blob, int size)
{
	int tempStatus = sqlite3_bind_blob(stmt, index, blob, size, SQLITE_STATIC);
	if(tempStatus == SQLITE_OK)
		statusCode = SqliteDatabase::STATUS_OK;
	else
		statusCode = SqliteDatabase::STATUS_SQL_ERROR;
	lastErrorMsg = std::to_string(tempStatus);
}
void SqliteStatement::bindInt(int index, int value)
{
	int tempStatus = sqlite3_bind_int(stmt, index, value);
	if(tempStatus == SQLITE_OK)
		statusCode = SqliteDatabase::STATUS_OK;
	else
		statusCode = SqliteDatabase::STATUS_SQL_ERROR;
	lastErrorMsg = std::to_string(tempStatus);
}
void SqliteStatement::bindInt64(int index, int64_t value)
{
	int tempStatus = sqlite3_bind_int64(stmt, index, value);
	if(tempStatus == SQLITE_OK)
		statusCode = SqliteDatabase::STATUS_OK;
	else
		statusCode = SqliteDatabase::STATUS_SQL_ERROR;
	lastErrorMsg = std::to_string(tempStatus);
}
void SqliteStatement::bindDouble(int index, double value)
{
	int tempStatus = sqlite3_bind_double(stmt, index, value);
	if(tempStatus == SQLITE_OK)
		statusCode = SqliteDatabase::STATUS_OK;
	else
		statusCode = SqliteDatabase::STATUS_SQL_ERROR;
	lastErrorMsg = std::to_string(tempStatus);
}
void SqliteStatement::bindText(int index, std::string value)
{
	int tempStatus = sqlite3_bind_text(stmt, index, value.c_str(), (int)value.size(), SQLITE_STATIC);
	if(tempStatus == SQLITE_OK)
		statusCode = SqliteDatabase::STATUS_OK;
	else
		statusCode = SqliteDatabase::STATUS_SQL_ERROR;
	lastErrorMsg = std::to_string(tempStatus);
}
void SqliteStatement::bindNull(int index)
{
	int tempStatus = sqlite3_bind_null(stmt, index);
	if(tempStatus == SQLITE_OK)
		statusCode = SqliteDatabase::STATUS_OK;
	else
		statusCode = SqliteDatabase::STATUS_SQL_ERROR;
	lastErrorMsg = std::to_string(tempStatus);
}
	
sqlite3_stmt* SqliteStatement::getStatement()
{
	return stmt;
}

bool SqliteStatement::isValid()
{
	return stmt != nullptr;
}

int SqliteStatement::getStatusCode()
{
	return statusCode;
}
std::string SqliteStatement::getStatusMessage()
{
	return lastErrorMsg;
}
#endif
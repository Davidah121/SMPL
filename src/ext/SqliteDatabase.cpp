#include "ext/SqliteDatabase.h"
#include "sqlite3.h"
#include <any>
#include <cstdint>
#include <fstream>
#include <cstring>
#include <StringTools.h>
#ifdef USE_SQLITE

struct DatabaseStructThing
{
	bool shouldIncludeHeader = false;
	std::vector<std::vector<std::string>>* data;
};

int SqliteDatabase::callback(void* data, int argc, char** argv, char** azColName)
{
	//assume data is a pointer to a std::vector<std::vector<std::string>>.
	//assume utf8
	DatabaseStructThing* k = (DatabaseStructThing*)data;
	std::vector<std::vector<std::string>>* table = k->data;

	if(table->size() == 0 && k->shouldIncludeHeader)
	{
		table->push_back( std::vector<std::string>() );
		for(int i=0; i<argc; i++)
		{
			table->back().push_back( azColName[i] );
		}
	}

	table->push_back( std::vector<std::string>() );
	for(int i=0; i<argc; i++)
	{
		if(argv[i] != nullptr)
			table->back().push_back( argv[i] );
		else
			table->back().push_back( "NULL" );
	}

	return 0;
}


SqliteData::SqliteData()
{
	type = TYPE_NULL;
}
SqliteData::~SqliteData()
{
	//special cases. Otherwise, ignore
	if(type == TYPE_STRING)
	{
		typedef std::string temp;
		std::string* ptr = ((std::string*)data.obj);
		ptr->~temp();
	}
	else if(type == TYPE_BLOB)
	{
		typedef std::vector<uint8_t> temp;
		std::vector<uint8_t>* ptr = ((std::vector<uint8_t>*)data.obj);
		ptr->~temp();
	}
}

const bool SqliteData::getNull()
{
	return type == TYPE_NULL;
}
const int8_t SqliteData::getByte()
{
	return data.integerNumber & 0xFF;
}
const int16_t SqliteData::getShort()
{
	return data.integerNumber & 0xFFFF;
}
const int32_t SqliteData::getInt()
{
	return data.integerNumber & 0xFFFFFFFF;
}
const int64_t SqliteData::getLong()
{
	return data.integerNumber;
}
const float SqliteData::getFloat()
{
	return data.floatNumber;
}
const double SqliteData::getDouble()
{
	return data.doubleNumber;
}
const std::string& SqliteData::getString()
{
	return *(std::string*)data.obj;
}
const std::vector<uint8_t>& SqliteData::getBlob()
{
	return *(std::vector<uint8_t>*)data.obj;
}
	
SqliteDatabase::SqliteDatabase()
{
	isValid = false;
	statusCode = STATUS_INVALID;
}

SqliteDatabase::SqliteDatabase(std::string filename)
{
	isValid = openDatabase(filename);
}

SqliteDatabase::SqliteDatabase(std::wstring filename)
{
	isValid = openDatabase(smpl::StringTools::toUTF8String(filename));
}

SqliteDatabase::~SqliteDatabase()
{
	close();
}

void SqliteDatabase::close()
{
	isValid = false;
	statusCode = STATUS_INVALID;
	if(databaseConnection!=nullptr)
		sqlite3_close(databaseConnection);
	databaseConnection = nullptr;
}

bool SqliteDatabase::valid()
{
	return isValid;
}

int SqliteDatabase::getStatusCode()
{
	return statusCode;
}

std::string SqliteDatabase::getStatusMessage()
{
	return lastErrorMsg;
}

bool SqliteDatabase::openDatabase(std::string filename)
{
	bool hadToCreate = true;
	this->filename = filename;
	std::fstream temp = std::fstream(filename, std::fstream::in | std::fstream::binary);
	if(temp.is_open())
		hadToCreate = false;
	temp.close();

	int error = sqlite3_open_v2(filename.c_str(), &databaseConnection, SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX, nullptr);
	if(error == SQLITE_OK)
	{
		if(hadToCreate)
			statusCode = STATUS_CREATED;
		else
			statusCode = STATUS_OK;
	}
	else
		statusCode = STATUS_INVALID;
	return (error >= SQLITE_OK);
}

std::vector<std::vector<std::string>> SqliteDatabase::executeCommandFromFile(std::string filename, bool includeHeader)
{
	if(!isValid)
		return {};
	std::fstream file = std::fstream(filename, std::fstream::binary | std::fstream::in);
	if(file.is_open())
	{
		std::string command = "";
		while (true)
		{
			int byte = file.get();
			if(!file.eof())
				command += byte;
			else
				break;
		}
		
		file.close();
		return executeCommand(command, includeHeader);
	}

	statusCode = STATUS_FILE_ERROR;
	return {};
}

std::vector<std::vector<std::string>> SqliteDatabase::executeCommandFromFile(std::wstring filename, bool includeHeader)
{
	if(!isValid)
		return {};
	
	std::string utfString = smpl::StringTools::toUTF8String(filename);
	std::fstream file = std::fstream(utfString, std::fstream::binary | std::fstream::in);
	if(file.is_open())
	{
		std::string command = "";
		while (true)
		{
			int byte = file.get();
			if(!file.eof())
				command += byte;
			else
				break;
		}
		
		file.close();
		return executeCommand(command, includeHeader);
	}

	statusCode = STATUS_FILE_ERROR;
	return {};
}

std::vector<std::vector<std::string>> SqliteDatabase::executeCommand(std::string command, bool includeHeader)
{
	return executeCommand(smpl::StringTools::toWideString(command), includeHeader);
}

std::vector<std::vector<std::string>> SqliteDatabase::executeCommand(std::wstring command, bool includeHeader)
{
	if(!isValid)
		return {};
	
	std::vector<std::vector<std::string>> results;
	std::string utfString = smpl::StringTools::toUTF8String(command);

	DatabaseStructThing k;
	k.shouldIncludeHeader = includeHeader;
	k.data = &results;

	int status = sqlite3_exec(databaseConnection, utfString.c_str(), SqliteDatabase::callback, &k, nullptr);
	
	if(status == SQLITE_OK || status == SQLITE_ROW || status == SQLITE_DONE)
		statusCode = STATUS_OK;
	else
		statusCode = STATUS_SQL_ERROR;

	lastErrorMsg = sqlite3_errmsg(databaseConnection);
	return results;
}

void SqliteDatabase::beginTransaction()
{
	executeCommand("BEGIN TRANSACTION;");
}

void SqliteDatabase::commitTransaction()
{
	executeCommand("COMMIT;");
}

void SqliteDatabase::rollbackTransaction()
{
	executeCommand("ROLLBACK;");
}

SqliteStatement SqliteDatabase::createStatment(std::string command)
{
	sqlite3_stmt* newStatement = nullptr;
	int status = sqlite3_prepare_v2(databaseConnection, command.c_str(), (int)command.size(), &newStatement, nullptr);
	
	SqliteStatement s = SqliteStatement(newStatement);

	if(status == SQLITE_OK || status == SQLITE_ROW || status == SQLITE_DONE)
	{
		statusCode = STATUS_OK;
		s.statusCode = STATUS_OK;
	}
	else
	{
		statusCode = STATUS_SQL_ERROR;
		s.statusCode = STATUS_SQL_ERROR;
	}

	s.lastErrorMsg = sqlite3_errmsg(databaseConnection);
	lastErrorMsg = s.lastErrorMsg;

	return s;
}

std::vector<SqliteData> SqliteDatabase::stepThroughStatement(SqliteStatement& s)
{
	int status = sqlite3_step(s.getStatement());
	
	lastErrorMsg = sqlite3_errmsg(databaseConnection);
	if(status == SQLITE_DONE)
	{
		statusCode = STATUS_DONE;
		s.statusCode = STATUS_DONE;
		return std::vector<SqliteData>();
	}
	else if(status == SQLITE_ROW)
	{
		statusCode = STATUS_DATA_RETRIEVED;
		s.statusCode = STATUS_DATA_RETRIEVED;
		int colSize = sqlite3_column_count(s.getStatement());
		std::vector<SqliteData> row = std::vector<SqliteData>(colSize);
		
		for(int colIndex=0; colIndex<colSize; colIndex++)
		{
			int type = sqlite3_column_type(s.getStatement(), colIndex);
			if(type == SQLITE_INTEGER)
			{
				row[colIndex] = sqlite3_column_int64(s.getStatement(), colIndex);
			}
			else if(type == SQLITE_FLOAT)
			{
				row[colIndex] = sqlite3_column_double(s.getStatement(), colIndex);
			}
			else if(type == SQLITE_TEXT)
			{
				const unsigned char* text = sqlite3_column_text(s.getStatement(), colIndex);
				row[colIndex] = std::string((const char*)text);
			}
			else if(type == SQLITE_BLOB)
			{
				//get size
				int actualByteSize = sqlite3_column_bytes(s.getStatement(), colIndex);
				std::vector<uint8_t> data = std::vector<uint8_t>(actualByteSize);

				//copy blob into vector that can be moved around
				const unsigned char* blob = (const unsigned char*)sqlite3_column_blob(s.getStatement(), colIndex);
				std::memcpy(data.data(), blob, actualByteSize);
				row[colIndex] = data;
			}

			//null otherwise
		}
		return row;
	}
	else
	{
		statusCode = STATUS_SQL_ERROR;
		s.statusCode = STATUS_SQL_ERROR;
		return std::vector<SqliteData>();
	}
}

void SqliteDatabase::resetStatement(SqliteStatement& s)
{
	int status = sqlite3_reset(s.getStatement());
	
	if(status == SQLITE_OK || status == SQLITE_ROW || status == SQLITE_DONE)
	{
		statusCode = STATUS_OK;
		s.statusCode = STATUS_OK;
	}
	else
	{
		statusCode = STATUS_SQL_ERROR;
		s.statusCode = STATUS_SQL_ERROR;
	}

	s.lastErrorMsg = sqlite3_errmsg(databaseConnection);
	lastErrorMsg = s.lastErrorMsg;
}

void SqliteDatabase::finalizeStatement(SqliteStatement& s)
{
	int status = sqlite3_finalize(s.getStatement());
	s.stmt = nullptr;
	
	if(status == SQLITE_OK || status == SQLITE_ROW || status == SQLITE_DONE)
	{
		statusCode = STATUS_OK;
		s.statusCode = STATUS_OK;
	}
	else
	{
		statusCode = STATUS_SQL_ERROR;
		s.statusCode = STATUS_SQL_ERROR;
	}

	s.lastErrorMsg = sqlite3_errmsg(databaseConnection);
	lastErrorMsg = s.lastErrorMsg;
}
#endif
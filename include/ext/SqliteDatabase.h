#pragma once
#define USE_SQLITE
#ifdef USE_SQLITE
#include<iostream>
#include<sqlite3.h>
#include<vector>

class SqliteDatabase;
class SqliteStatement;
class SqliteData;

class SqliteStatement
{
public:
	static const int TYPE_INT = SQLITE_INTEGER;
	static const int TYPE_NULL = SQLITE_NULL;
	static const int TYPE_DOUBLE = SQLITE_FLOAT;
	static const int TYPE_TEXT = SQLITE_TEXT;
	static const int TYPE_BLOB = SQLITE_BLOB;
	
	SqliteStatement(const SqliteStatement& other) = delete;
	SqliteStatement& operator=(const SqliteStatement& other) = delete;

	SqliteStatement(SqliteStatement&& other) = default;
	SqliteStatement& operator=(SqliteStatement&& other) = default;
	~SqliteStatement();

	/**
	* @brief Attempts to bind a blob to the given index. That index being the location of the "?" in the statement.
	*		Note that the binding may fail so checking the status code is kind of useful.
	*			The actual error code from sqlite will be put into the status message and will need to be converted back and looked up.
	* 
	* @param index 
	* @param blob 
	* @param size 
	*/
	void bindBlob(int index, const void* blob, int size);

	/**
	* @brief Attempts to bind an integer to the given index. That index being the location of the "?" in the statement.
	*		Note that the binding may fail so checking the status code is kind of useful.
	*			The actual error code from sqlite will be put into the status message and will need to be converted back and looked up.
	* 
	* @param index 
	* @param value 
	*/
	void bindInt(int index, int value);

	/**
	* @brief Attempts to bind a long to the given index. That index being the location of the "?" in the statement.
	*		Note that the binding may fail so checking the status code is kind of useful.
	*			The actual error code from sqlite will be put into the status message and will need to be converted back and looked up.
	* 
	* @param index 
	* @param value 
	*/
	void bindInt64(int index, int64_t value);
	
	/**
	* @brief Attempts to bind a double to the given index. That index being the location of the "?" in the statement.
	*		Note that the binding may fail so checking the status code is kind of useful.
	*			The actual error code from sqlite will be put into the status message and will need to be converted back and looked up.
	* 
	* @param index 
	* @param value 
	*/
	void bindDouble(int index, double value);

	/**
	* @brief Attempts to bind text (assumed to be utf-8 text) to the given index. That index being the location of the "?" in the statement.
	*		Note that the binding may fail so checking the status code is kind of useful.
	*			The actual error code from sqlite will be put into the status message and will need to be converted back and looked up.
	* 
	* @param index 
	* @param value 
	*/
	void bindText(int index, std::string value);

	/**
	* @brief Attempts to bind null to the given index. That index being the location of the "?" in the statement.
	*		Note that the binding may fail so checking the status code is kind of useful.
	*			The actual error code from sqlite will be put into the status message and will need to be converted back and looked up.
	* 
	* @param index 
	*/
	void bindNull(int index);
	
	sqlite3_stmt* getStatement();
	bool isValid();

	int getStatusCode();
	std::string getStatusMessage();
private:
	friend SqliteDatabase;
	SqliteStatement(sqlite3_stmt* stmt);
	sqlite3_stmt* stmt = nullptr;
	int statusCode;
	std::string lastErrorMsg;
};

class SqliteData
{
public:
	static const int TYPE_NULL = 0;
	static const int TYPE_BYTE = 1;
	static const int TYPE_SHORT = 2;
	static const int TYPE_INT = 3;
	static const int TYPE_LONG = 4;
	static const int TYPE_FLOAT = 5;
	static const int TYPE_DOUBLE = 6;
	static const int TYPE_STRING = 7;
	static const int TYPE_BLOB = 7;
	
	SqliteData();
	SqliteData(int8_t v);
	SqliteData(int16_t v);
	SqliteData(int32_t v);
	SqliteData(int64_t v);
	SqliteData(float v);
	SqliteData(double v);
	SqliteData(std::string v);
	SqliteData(std::vector<uint8_t> v);
	~SqliteData();

	const bool getNull();
	const int8_t getByte();
	const int16_t getShort();
	const int32_t getInt();
	const int64_t getLong();
	const float getFloat();
	const double getDouble();
	const std::string& getString();
	const std::vector<uint8_t>& getBlob();

private:
	int type = 0;
	union
	{
		char obj[24];
		int64_t integerNumber;
		float floatNumber;
		double doubleNumber;
	} data;
};

class SqliteDatabase
{
public:
	static const int STATUS_OK = 0;
	static const int STATUS_FILE_ERROR = -1;
	static const int STATUS_SQL_ERROR = -2;
	static const int STATUS_INVALID = -3;
	static const int STATUS_NOT_FOUND = -4;
	static const int STATUS_CREATED = 1;
	static const int STATUS_DATA_RETRIEVED = 2;
	static const int STATUS_DONE = 3;
	
	/**
	* @brief Construct an empy Sqlite Database object
	* 
	*/
	SqliteDatabase();

	/**
	* @brief Construct a new Sqlite Database object by opening the corresponding file as a database.
	*		Note that if the file does not already exist, an empty database will be created in its place.
	*		Note: Opened with the flag "SQLITE_OPEN_FULLMUTEX" so multiple threads may share this class without needing mutexes.
	* 			This means that each thread is able to write single threaded code and it work out fine.
	*
	* @param filename 
	*/
	SqliteDatabase(std::string filename);
	
	/**
	* @brief Construct a new Sqlite Database object by opening the corresponding file as a database.
	*		Note that if the file does not already exist, an empty database will be created in its place.
	*		Note: Opened with the flag "SQLITE_OPEN_FULLMUTEX" so multiple threads may share this class without needing mutexes.
	*			This means that each thread is able to write single threaded code and it work out fine.
	* 
	* @param filename 
	*/
	SqliteDatabase(std::wstring filename);
	SqliteDatabase(const SqliteDatabase& other) = delete;
	void operator=(const SqliteDatabase& other) = delete;
	SqliteDatabase(SqliteDatabase&& other) = default;
	SqliteDatabase& operator=(SqliteDatabase&& other) = default;

	/**
	* @brief Destroy the Sqlite Database object
	* 
	*/
	~SqliteDatabase();

	/**
	* @brief Determines if the database is in a valid state. (not destroyed)
	* 
	* @return true 
	* @return false 
	*/
	bool valid();

	/**
	* @brief Get the Status Code for the database. This contains some error information but these errors can be modified by other threads
	*		The best use case for this is in single threaded code
	* 
	* @return int 
	*/
	int getStatusCode();

	/**
	* @brief Get the status message for the database. This contains more information about the status code (like its cause) but it can be modified by other threads.
	*		Useful in single threaded code.
	* 
	* @return std::string 
	*/
	std::string getStatusMessage();

	/**
	* @brief Attempts to execute sql commands from a file and give the results. This method may include the header if requested but all
	*		data obtained this way will be strings that need to be converted back into their correct datatype.
	*		This method also modifies the global status code and status message.
	*		
	*		Great for quick queries where the status code is not useful or in single threaded applications.
	* 
	* @param file 
	* @param includeHeader 
	* @return std::vector<std::vector<std::string>> 
	*/
	std::vector<std::vector<std::string>> executeCommandFromFile(std::string file, bool includeHeader = false);

	/**
	* @brief Attempts to execute sql commands from a file and give the results. This method may include the header if requested but all
	*		data obtained this way will be strings that need to be converted back into their correct datatype.
	*		This method also modifies the global status code and status message.
	*		
	*		Great for quick queries where the status code is not useful or in single threaded applications.
	* 
	* @param file 
	* @param includeHeader 
	* @return std::vector<std::vector<std::string>> 
	*/
	std::vector<std::vector<std::string>> executeCommandFromFile(std::wstring file, bool includeHeader = false);

	/**
	* @brief Attempts to execute sql commands and give the results. This method may include the header if requested but all
	*		data obtained this way will be strings that need to be converted back into their correct datatype.
	*		This method also modifies the global status code and status message.
	*		
	*		Great for quick queries where the status code is not useful or in single threaded applications.
	* 
	* @param file 
	* @param includeHeader 
	* @return std::vector<std::vector<std::string>> 
	*/
	std::vector<std::vector<std::string>> executeCommand(std::string command, bool includeHeader = false);

	/**
	* @brief Attempts to execute sql commands and give the results. This method may include the header if requested but all
	*		data obtained this way will be strings that need to be converted back into their correct datatype.
	*		This method also modifies the global status code and status message.
	*		
	*		Great for quick queries where the status code is not useful or in single threaded applications.
	* 
	* @param file 
	* @param includeHeader 
	* @return std::vector<std::vector<std::string>> 
	*/
	std::vector<std::vector<std::string>> executeCommand(std::wstring command, bool includeHeader = false);

	/**
	* @brief Create a SqliteStatment object from the provided command that can be executed.
	*		Note that the statement contains its own status code and message making it highly useful in multi-threaded scenarios
	*			assuming the database automatically handles multiple threads accessing it at once which it should.
	*		This just creates the statement. Not executes it.
	* 
	* @param command 
	* @return SqliteStatement 
	*/
	SqliteStatement createStatment(std::string command);

	/**
	* @brief Executes one step of the SqliteStatement.
	* 		Upon completion, the status code STATUS_DONE will be set.
	* 		Returns a row of data. Each SqliteData object contains its actual type and may be null type
	*
	* @param s 
	* @return std::vector<SqliteData> 
	*/
	std::vector<SqliteData> stepThroughStatement(SqliteStatement& s);

	/**
	* @brief Resets a statement. This puts it back to the start of the statement so if you were stepping through multiple rows,
	*		it will reset back to the 1st row.
	* 
	* @param s 
	*/
	void resetStatement(SqliteStatement& s);

	/**
	* @brief Ends a statement. After doing this, it should not be used (it can't be used) as it is invalid. 
	*		This will be done for all statements once their destructor is called anyways.
	* 
	* @param s 
	*/
	void finalizeStatement(SqliteStatement& s);
	
	/**
	* @brief Executes the command "BEGIN TRANSACTION;"
	*		Transactions are useful since everything that occurs within them can be rolled back.
	* 
	*/
	void beginTransaction();

	/**
	* @brief Commits a transaction in progress.
	*		This will finalize the changes from the start of the transaction if there were any.
	* 
	*/
	void commitTransaction();

	/**
	* @brief Rolls back a transaction in progress.
	*		This will remove any changes made from the start of the transaction.
	* 
	*/
	void rollbackTransaction();
	
private:
	/**
	* @brief Attempts to open the database at the specified file. No in-memory database allowed yet.
	* 
	* @param filename 
	* @return true 
	* @return false 
	*/
	bool openDatabase(std::string filename);
	
	/**
	* @brief Closes the database connection. Will be called upon class destruction
	* 
	*/
	void close();

	static int callback(void* data, int argc, char** argv, char** azColName);

	sqlite3* databaseConnection = nullptr;
	bool isValid = false;
	int statusCode = 0;
	std::string lastErrorMsg = "";
	std::string filename;
};
#endif
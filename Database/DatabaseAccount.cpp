#include "DatabaseAccount.h"
#include <algorithm>


std::string WStringToUTF8(const std::wstring& wstr) {
	if (wstr.empty()) return std::string();

	//int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	if (size_needed <= 0) return  std::string();

	std::string utf8_str(size_needed - 1, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, utf8_str.data(), size_needed, nullptr, nullptr);
	//WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &utf8_str[0], size_needed, NULL, NULL);

	return utf8_str;
}

std::wstring UTF8ToWString(const std::string& utf8Str) {
	if (utf8Str.empty()) return L"";

	int size_needed = MultiByteToWideChar(65001, 0, utf8Str.c_str(), -1, NULL, 0);
	if (size_needed <= 0) return L"";

	std::wstring wstr(size_needed - 1, 0);
	MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wstr[0], size_needed);
	return wstr;
}


DatabaseAccount::DatabaseAccount(int maxRow):
	conn(nullptr), res(nullptr), row(nullptr), isTransaction(false), isDirty(false), maxRow(maxRow)
{}

DatabaseAccount::~DatabaseAccount() 
{
	Close();
}

bool DatabaseAccount::Connect(const char* server, const char* user, const char* password, const char* database)
{
	if (conn) return false;

	conn = mysql_init(nullptr);
	if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) 
	{
		return false;
	}
	mysql_set_character_set(conn, "utf8");

	DBinfo.LogIn(server, user, password, database);
	DBinfo.Connect();
	
	return true;
}

bool DatabaseAccount::Connect(const std::string& server, const std::string& user, const std::string& password, const std::string& database)
{
	return Connect(server.c_str(), user.c_str(), password.c_str(),
		database == "None" ? nullptr : database.c_str());
}

bool DatabaseAccount::ReConnect()
{
	Close();
	if (!DBinfo.IsLogIn()) return false;
	return Connect(DBinfo.GetServer(), DBinfo.GetID(), DBinfo.GetPW(), DBinfo.GetDatabase());
}

void DatabaseAccount::Close()
{
	DBinfo.Close();
	freeResult();
	if (conn)
	{
		mysql_close(conn);
		conn = nullptr;
	}
}



bool DatabaseAccount::IsUseQuery(const std::string query)
{
	std::string lower = query;
	std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
	return lower.rfind("use ", 0) == 0;
}

std::string DatabaseAccount::ExtractDatabaseName(const std::string& query)
{
	std::string db = query.substr(4); // "use "

	// 세미콜론 제거
	if (!db.empty() && db.back() == ';')
		db.pop_back();

	return db;
}


my_ulonglong DatabaseAccount::ExecuteQuery(const std::string& query)
{
	if (conn == nullptr) return -1;
	freeResult(); // 이전 결과 정리

	if (mysql_query(conn, query.c_str()) != 0) return -1;
	res = mysql_store_result(conn);
	bool isOK = mysql_errno(conn) == 0;
	if (isOK)
	{
		if (IsUseQuery(query)) DBinfo.SetDatabaseName(ExtractDatabaseName(query));
		if (isTransaction && mysql_affected_rows(conn) > 0)	isDirty = true;
	}

	return  mysql_affected_rows(conn); // 수정된 column 갯수를 return. 에러 발생 시 -1
}

my_ulonglong DatabaseAccount::ExecuteQuery(const std::wstring& query)
{
	return ExecuteQuery(WStringToUTF8(query));
}

bool DatabaseAccount::StartTransaction()
{
	if (isTransaction == true) return false;
	if (ExecuteQuery("START TRANSACTION;") == -1) return false;
	isTransaction = true;
	return true;
}
bool DatabaseAccount::Commit()
{
	if (ExecuteQuery("COMMIT;") == -1) return false;
	if (isTransaction) isTransaction = false;
	isDirty = false;
	return true;
}
bool DatabaseAccount::Rollback()
{
	if (ExecuteQuery("ROLLBACK;") == -1) return false;
	if (isTransaction) isTransaction = false;
	isDirty = false;
	return true;
}

std::string DatabaseAccount::GetLastError()
{
	if (conn == nullptr) return "No connection";
	unsigned int errNo = mysql_errno(conn);
	const char* errMsg = mysql_error(conn);
	std::string error =	"Error " + std::to_string(errNo) + ": " + errMsg;
	return error;
}
std::wstring DatabaseAccount::GetLastErrorW()
{
	return UTF8ToWString(GetLastError());
}

MYSQL_ROW DatabaseAccount::fetchRow()
{
	return res == nullptr ? nullptr : mysql_fetch_row(res);
}

MYSQL_FIELD* DatabaseAccount::getFieldName() { return res == nullptr ? nullptr : mysql_fetch_fields(res); }
unsigned int DatabaseAccount::getFieldNum() { return res == nullptr ? 0 : mysql_num_fields(res); }

void DatabaseAccount::freeResult()
{
	if (res) 
	{
		mysql_free_result(res);
		res = nullptr;
	}
	row = nullptr;
}


#ifndef _DATABASEACCOUNT_H
#define _DATABASEACCOUNT_H

#include "..\stdafx.h"
#include "mysql.h"
#include <Windows.h>

extern std::string WStringToUTF8(const std::wstring& wstr);
extern std::wstring UTF8ToWString(const std::string& utf8Str);



// 실제 데이터베이스를 다루는 account 클래스
class DatabaseUser
{
	std::string server;
	std::string ID;
	std::string PW;
	std::string database;

	bool isLogIn;		// 로그인된 상태인가(계정 정보가 있는가)
	bool isConnected;	// 실제로 연결된 상태인가(connect 된 상태인가)

public:
	DatabaseUser() : isLogIn(false), isConnected(false)
	{
	}
	~DatabaseUser()
	{
		Close();
	}

	void LogIn(const char* server, const char* user, const char* password, const char* database)
	{
		this->server = server ? server : "";
		this->ID = user ? user : "";
		this->PW = password ? password : "";
		this->database = database ? database : "";
		isLogIn = true;
	}

	void LogIn(std::string server, std::string ID, std::string PW, std::string database)
	{
		LogIn(server.c_str(), ID.c_str(), PW.c_str(), database.c_str());
	}

	bool Connect()
	{
		if (!isLogIn) return false;
		isConnected = true;
		return true;
	}
	void DisConnect()
	{
		isConnected = false;
	}

	const std::string& GetServer() const { return server; }
	const std::string& GetID() const { return ID; }
	const std::string& GetPW() const { return PW; }
	const std::string& GetDatabase() const { return database; }

	bool IsLogIn() const { return isLogIn; }
	bool IsConnect() const { return isConnected; } 
	
	void SetDatabaseName(const std::string& name) { database = name; }

	void Close()
	{
		server.clear();
		ID.clear();
		PW.clear();
		database.clear();
		isLogIn = false;
		isConnected = false;
	}
};

class DatabaseAccount
{
	DatabaseUser DBinfo;

	MYSQL* conn;
	MYSQL_RES* res;
	MYSQL_ROW row;

	bool isTransaction;
	bool isDirty;

	std::vector<std::wstring> prevQuerys;

	bool IsUseQuery(const std::string query);
	std::string ExtractDatabaseName(const std::string& query);
public:
	DatabaseAccount();
	virtual ~DatabaseAccount();

	bool Connect(const char* server, const char* user, const char* password, const char* database);
	bool Connect(const std::string& server, const std::string& user, const std::string& password, const std::string& database); // database를 연결하지 않을 경우 "None" 을 입력하세요
	bool ReConnect();
	void Close();

	my_ulonglong ExecuteQuery(const std::string& query); // 실제 쿼리문 실행
	my_ulonglong ExecuteQuery(const std::wstring& query); // 실제 쿼리문 실행

	std::string GetLastError();
	std::wstring GetLastErrorW();

	MYSQL_ROW fetchRow(); // 쿼리문 결과를 row 형태로 가져 옴
	MYSQL_ROW fetchRowFromOutside(MYSQL_RES* resOutside) {return resOutside == nullptr ? nullptr : mysql_fetch_row(resOutside);}
	MYSQL_RES* GetResult() { return res; }
	void freeResult(); // 결과 free

	// query 실행 후 호출하기
	MYSQL_FIELD* getFieldName();
	unsigned int getFieldNum();


	bool StartTransaction();
	bool Commit();
	bool Rollback();
	bool IsDirty() const { return isDirty; }

	my_ulonglong SetForeignKeyCheck(bool flag)
	{
		return flag = true ? ExecuteQuery("SET FOREIGN_KEY_CHECKS = 1;") : ExecuteQuery("SET FOREIGN_KEY_CHECKS = 0;");
	}

	std::wstring GetDatabaseName() { return UTF8ToWString(DBinfo.GetDatabase()); }

	bool IsConnected() const { return conn != nullptr; }
};


#endif


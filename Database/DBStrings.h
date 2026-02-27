#ifndef _DBSTRINGS_H
#define _DBSTRINGS_H

#include <string>
struct DBQueryExamples
{
	// User 권한
	std::wstring createUser = L"CREATE USER IF NOT EXISTS defaultUser@localhost IDENTIFIED BY 'defaultPW'";
	std::wstring resetPW = L"ALTER USER defaultUser@localhost IDENTIFIED BY 'newPW';";
	std::wstring grantDBtoUser = L"GRANT ALL PRIVILEGES ON defaultDB.* TO defaultUser@localhost WITH GRANT OPTION;";
	std::wstring dropUser = L"DROP USER IF EXISTS defaultUser@localhost;";

	// DB 생성
	std::wstring createDB = L"CREATE DATABASE IF NOT EXISTS defaultDB;";
	std::wstring dropDB = L"DROP DATABASE IF EXISTS defaultDB;";

	// Table 생성
	std::wstring createTable = L"CREATE TABLE defaultTable (id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(50) NOT NULL, created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP);";
	
	// 기본 query
	std::wstring select = L"SELECT * FROM defaultTable;";
	std::wstring insert = L"INSERT INTO defaultTable (name) VALUES('test');";
	std::wstring update = L"UPDATE defaultTable SET name = 'newName' WHERE id = 1;";
	std::wstring del = L"DELETE FROM defaultTable WHERE id = 1;";

	std::wstring describeTable = L"DESCRIBE defaultTable;";
	std::wstring showTable = L"SHOW TABLES;";
	std::wstring showDB = L"SHOW DATABASES;";

	std::wstring showProcessList = L"SHOW PROCESSLIST;"; // 실행 중인 세션 목록을 보여 줌
	std::wstring showTableStatus = L"SHOW TABLE STATUS;"; // Table들의 물리적 상태 정보(이름, 크기, 시간, auto increment 등등)
	
	std::wstring transaction = L"START TRANSACTION; UPDATE users SET gold = gold - 100 WHERE user_id = 1; SAVEPOINT sp1; UPDATE users SET gold = gold + 100 WHERE user_id = 2; ROLLBACK TO sp1; COMMIT;"
};
#endif
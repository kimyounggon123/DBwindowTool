#ifndef _DBSTRINGS_H
#define _DBSTRINGS_H

#include <Windows.h>
#include <set>
#include <vector>
#include <string>

struct KeywordGroup
{
	std::set<std::wstring> words;
	COLORREF color;
	bool isBold;
};

struct DBQueryExamples
{
	KeywordGroup blueKeyword =
	{ { L"SELECT", L"FROM", L"WHERE", L"INSERT", L"UPDATE", L"DELETE", L"USE",
		L"JOIN", L"INNER", L"OUTER", L"ON", L"LEFT", L"RIGHT", L"FULL", L"USING",
		L"DESCRIBE", L"LIMIT", L"EXPLAIN", L"ORDER BY"}, RGB(30, 144, 255), true};

	KeywordGroup purpleKeyword =
	{ { L"AND", L"OR", L"NOT", L"IN", L"EXISTS", L"BETWEEN", L"LIKE", L"IS", L"NULL",
			L"AS", L"DISTINCT", L"ANY", L"ALL", L"SOME",
			L"CASE", L"WHEN", L"THEN", L"ELSE", L"END", L"MOD", L"DIV", L"XOR",
			L"+", L"-", L"*", L"/", L"<", L">", L"=", L"<>", L"!=", L"%"}, RGB(150, 105, 225), true};
	
	KeywordGroup indigoKeyword = 
	{ { L"INT", L"INTEGET", L"TINYINT", L"BIGINT", L"DECIMAL", L"NUMERIC", L"FLOAT", L"DOUBLE",
			L"CHAR", L"VARCHAR", L"TEXT" ,L"LONGTEXT", L"NVARCHAR",
			L"DATETIME", L"TIMESTAMP", L"TIME", L"BOOLEAN",
			L"AUTO_INCREMENT", L"DEFAULT", L"PRIVATE KEY", L"FOREIGN KEY", L"ASC", L"DESC"},RGB(35, 35, 225), true};

	KeywordGroup yellowKeyword =
	{ { L"CREATE", L"ALTER", L"DROP", L"TRUNCATE", L"NAME",
			L"COUNT", L"SUM", L"AVG", L"MIN", L"MAX"}, RGB(130, 130, 0), true };

	std::vector<KeywordGroup> keywordGroups = { blueKeyword, indigoKeyword, purpleKeyword, yellowKeyword };


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
	std::wstring truncate = L"TRUNCATE TABLE defaultTable;";

	std::wstring describeTable = L"DESCRIBE defaultTable;";
	std::wstring showTable = L"SHOW TABLES;";
	std::wstring showDB = L"SHOW DATABASES;";

	std::wstring showProcessList = L"SHOW PROCESSLIST;"; // 실행 중인 세션 목록을 보여 줌
	std::wstring showTableStatus = L"SHOW TABLE STATUS;"; // Table들의 물리적 상태 정보(이름, 크기, 시간, auto increment 등등)
	
	std::wstring transaction = L"START TRANSACTION; UPDATE users SET gold = gold - 100 WHERE user_id = 1; SAVEPOINT sp1; UPDATE users SET gold = gold + 100 WHERE user_id = 2; ROLLBACK TO sp1; COMMIT;";
		
	/*
	(view 생성)
			CREATE VIEW 뷰_이름 AS
			SELECT 컬럼1, 컬럼2, ...
			FROM 테이블명
			WHERE 조건;

		CREATE VIEW v_student_results AS
		SELECT
			s.name AS student_name,
			s.class,
			sc.subject,
			sc.score
		FROM students s
		INNER JOIN scores sc ON s.id = sc.student_id
		WHERE sc.score >= 60; -- 60점 이상만 보는 뷰
	*/
};
#endif
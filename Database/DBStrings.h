#ifndef _DBSTRINGS_H
#define _DBSTRINGS_H

#include <string>
struct DBQueryExamples
{
	std::wstring createUser = L"CREATE USER defaultUser@localhost IDENTIFIED BY 'defaultPW'";
	std::wstring createDB = L"CREATE DATABASE defaultDB;";
	std::wstring grantDB = L"GRANT ALL PRIVILEGES ON defaultDB.* TO defaultUser@localhost WITH GRANT OPTION;";

	std::wstring select = L"SELECT * FROM defaultTable;";
};
#endif
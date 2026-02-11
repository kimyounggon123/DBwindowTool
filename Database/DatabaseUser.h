#ifndef _DATABASEUSER_H
#define _DATABASEUSER_H

#include <string>

class DatabaseUser
{
	std::string ID;
	std::string PW;
	std::string database;
	
	bool isLogIn;
	bool isConnected;


	std::string query;
public:
	DatabaseUser(): isLogIn(false), isConnected(false)
	{}
	~DatabaseUser()
	{}

	void Connect(std::string ID, std::string PW, std::string database);

	void UpdateQuery(const char* query);
	const std::string& GetQuery() { return query; }


};

#endif
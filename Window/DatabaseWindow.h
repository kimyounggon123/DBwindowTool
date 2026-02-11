#ifndef _DATABASEDIALOG_H
#define _DATABASEDIALOG_H


#include "WindowEX.h"
#include "DatabaseUser.h"
class DatabaseWindow : public WindowEX
{
	bool isAdmin;
	
public:
	DatabaseWindow(WindowInformations info): WindowEX(info)
	{}
	~DatabaseWindow()
	{}

	
};


#endif

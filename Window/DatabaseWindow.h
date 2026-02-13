#ifndef _DATABASEDIALOG_H
#define _DATABASEDIALOG_H


#include "WindowEX.h"
#include "DatabaseUser.h"
class DatabaseWindow : public WindowEX
{
	bool isAdmin;
	
	WindowUI* editUI;

public:
	DatabaseWindow(WindowInformations info, bool isAdmin):
		WindowEX(info), isAdmin(isAdmin), editUI(nullptr)
	{}

	~DatabaseWindow()
	{
		SAFE_FREE(editUI);
	}

	bool InitializeWindow(const wchar_t* title, WNDPROC wndProc) override;
	static LRESULT CALLBACK DBMain(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};


#endif

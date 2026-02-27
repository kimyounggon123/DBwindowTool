#ifndef _DATABASEDIALOG_H
#define _DATABASEDIALOG_H


#include "WindowEX.h"
#include "DatabaseAccount.h"
#include "DBStrings.h"

#include <commctrl.h> // ListView »ç¿ë
#pragma comment(lib, "Comctl32.lib")



class DatabaseWindow : public WindowEX
{
	static DatabaseAccount* account;
	static DBQueryExamples queryExample;
	INITCOMMONCONTROLSEX icex;
	
	bool isAdmin;
	
	void InitializeUI();

	static void WM_CREATE_FUNC(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static void LogIn(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static void LogOut(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static void SendQuery(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static void SanitizeForListView(std::wstring& str);
	static void ShowResultMsg(const std::wstring& str, bool isError);
public:
	DatabaseWindow(WindowInformations info, bool isAdmin) :
		WindowEX(info), isAdmin(isAdmin), icex{}
	{}

	~DatabaseWindow()
	{}

	bool InitializeWindow(const wchar_t* title, WNDPROC wndProc) override;
	static LRESULT CALLBACK DBMain(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void Shutdown() override;
};


#endif

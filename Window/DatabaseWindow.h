#ifndef _DATABASEDIALOG_H
#define _DATABASEDIALOG_H

#include "resource.h"
#include "WindowEX.h"
#include "DatabaseAccount.h"
#include "DBStrings.h"
#include <cstdint>
#include <commctrl.h> // ListView »ç¿ë
#pragma comment(lib, "Comctl32.lib")


class DatabaseWindow : public WindowEX
{
	static HMENU hMenuBar;
	static DatabaseAccount* account; 

	static DBQueryExamples queryExample;
	
	INITCOMMONCONTROLSEX icex;
	
	bool isAdmin;

	void InitializeUI();

	static void SanitizeForListView(std::wstring& str)
	{
		str.erase( 
			std::remove_if(str.begin(), str.end(),
				[](wchar_t c)
				{
					return c == L'\r' || c == L'\n';
				}),
			str.end()
		);
	}

	// DBmain
	static void WM_CREATE_FUNC(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static void LogOut(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static bool WorkQueryProcess(const std::wstring& query);
	static void SendQuery(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static void ShowResultMsg(const std::wstring& str, bool isError = false);
	static void LogIn(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static bool RefreshTree();
	static LRESULT CALLBACK RichEditSubProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

	enum class TransactionType
	{
		Start,
		Commit,
		Rollback
	};

	static void SetTransactionMode(const TransactionType& type);

	static void NotifyTreeClick(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static void NotifyTableMaking(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static uint32_t NotifyTableColoring(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static void SetTransactionMode(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static void RefreshAll();
	static void ApplySqlHighlight();

public:
	DatabaseWindow(WindowInformations info, bool isAdmin) :
		WindowEX(info), isAdmin(isAdmin), icex{}
	{}

	~DatabaseWindow()
	{}

	bool InitializeWindow(const wchar_t* title, WNDPROC wndProc) override;

	//static LRESULT CALLBACK DBLogIn(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK DBMain(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	
	void Shutdown() override;
};


#endif

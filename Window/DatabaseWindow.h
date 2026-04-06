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
	static void WM_CREATE_FUNC(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	///////////////////////////////// Account /////////////////////////////////
	static void LogIn(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static void LogOut(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	///////////////////////////////// Query process /////////////////////////////////
	static my_ulonglong WorkQueryProcess(const std::wstring& query);
	static void SendQuery(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	///////////////////////////////// Transaction /////////////////////////////////
	enum class TransactionType
	{
		Start,
		Commit,
		Rollback
	};
	static void SetTransactionMode(const TransactionType& type);
	static void SetTransactionMode(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static void DrawAutoTransaction(LPDRAWITEMSTRUCT lpDrawItem, HDC hdc, RECT rect);

	///////////////////////////////// Tree /////////////////////////////////
	static bool RefreshTree();
	static void NotifyTreeClick(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static void NotifyTableMaking(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static uint32_t NotifyTableColoring(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


	///////////////////////////////// Edit box /////////////////////////////////
	static LRESULT CALLBACK RichEditSubProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	static void ApplySqlHighlight();

	///////////////////////////////// Log /////////////////////////////////
	static void WriteMsg(const std::wstring str, bool isError = false);
	static void WriteQueryResult(const std::wstring query, double ms = 0.0);
	static void UpdateUsedTimeAndColumns(double ms, my_ulonglong columns);

	///////////////////////////////// Others /////////////////////////////////
	static void RefreshAll();

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

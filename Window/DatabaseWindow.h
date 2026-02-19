#ifndef _DATABASEDIALOG_H
#define _DATABASEDIALOG_H


#include "WindowEX.h"
#include "DatabaseUser.h"


#include <commctrl.h> // ListView »ç¿ë
#pragma comment(lib, "Comctl32.lib")


class TableUI : public WindowUI
{
	int rowSize;
	int columnSize;
	
public:
	TableUI(const std::wstring& imagePath, const Position& pos) :
		WindowUI(imagePath, pos), rowSize(0), columnSize(0)
	{}
	TableUI(const std::wstring& imagePath, const Transform2DINT& transform) :
		WindowUI(imagePath, transform), rowSize(0), columnSize(0)
	{}
	~TableUI() = default;

	bool Create(DWORD ExStyle, LPCWSTR lpClassName, LPCWSTR lpWinName, DWORD dwStyle, HWND parentsWindow, HMENU id, HINSTANCE hInstance) override
	{
		
		SendMessageW(winUI,
			LVM_SETEXTENDEDLISTVIEWSTYLE,
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES,
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		return WindowUI::Create(ExStyle, lpClassName, lpWinName, dwStyle, parentsWindow, id, hInstance);
	}
	void Resize(int width)
	{
		//double part = columnSize;
		for (int i = 0; i < columnSize; i++)
		{
			ListView_SetColumnWidth(winUI, i, width * 0.1);
		}
	}
	void SetColumns(const std::vector<std::wstring>& cols);
	void AddRow(const std::vector<std::wstring>& data);
	void Clear();
};

class DatabaseWindow : public WindowEX
{
	INITCOMMONCONTROLSEX icex;
	bool isAdmin;
	
	void InitializeUI();

	static void WM_CREATE_FUNC(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	DatabaseWindow(WindowInformations info, bool isAdmin) :
		WindowEX(info), isAdmin(isAdmin), icex{}
	{}

	~DatabaseWindow()
	{
	}

	bool InitializeWindow(const wchar_t* title, WNDPROC wndProc) override;
	static LRESULT CALLBACK DBMain(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void Shutdown() override;
};


#endif

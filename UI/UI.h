#ifndef _UI_H
#define _UI_H

#include "..\stdafx.h"
#include "..\Utils\Transform.h"
#include <string>
#include "mysql.h"
class UIElement
{
	int ID;
	bool activated;

	std::wstring imagePath;

protected:
	Transform2DINT transform;

	std::vector<UIElement*> children;
public:
	UIElement(const std::wstring& imagePath, const Position& pos) :
		ID(0),
		activated(true),
		transform(pos, {10, 10}),
		imagePath(imagePath)
	{}
	UIElement(const std::wstring& imagePath, const Transform2DINT& transform) :
		ID(0),
		activated(true),
		transform(transform),
		imagePath(imagePath)
	{}
	virtual ~UIElement() {}


	int GetID() const { return ID; }
	void SetID(int ID) { this->ID = ID; }

	const Transform2DINT& GetTransform() const { return transform; }
	void SetPosition(const Position& pos) { transform.pos = pos; }
	void SetScale(const Vector2Int& scale) { transform.scale = scale; }

	const std::wstring& GetImage() const { return imagePath; }

	bool IsAble() const { return activated; }
	bool SetAble(bool switchThis) { activated = switchThis; }

	//bool AddChild(UIElement* child);
	//bool RemoveChild(int ID);
};


#include <Windows.h>
class WindowUI : public UIElement
{
protected:
	HWND winUI;
public:
	WindowUI(const std::wstring& imagePath, const Position& pos) :
		UIElement(imagePath, pos), winUI(nullptr)
	{
	}
	WindowUI(const std::wstring& imagePath, const Transform2DINT& transform) :
		UIElement(imagePath, transform), winUI(nullptr)
	{}

	virtual bool Create(DWORD ExStyle, LPCWSTR lpClassName, LPCWSTR lpWinName, DWORD dwStyle, HWND parentsWindow, HMENU id, HINSTANCE hInstance)
	{
		if (winUI != nullptr) return true;

		winUI = CreateWindowExW(
			ExStyle,
			lpClassName, // 2번째 
			lpWinName,
			dwStyle,
			transform.pos.x, transform.pos.y,
			transform.scale.x, transform.scale.y,
			parentsWindow,
			id,                 // 버튼 ID
			hInstance,
			this
		);

		return winUI != NULL;
	}

	bool Create(DWORD ExStyle, LPCWSTR lpClassName, LPCWSTR lpWinName, DWORD dwStyle,
		Position pos, Vector2Int size,
		HWND parentsWindow, HMENU id, HINSTANCE hInstance)
	{
		transform.pos = pos;
		transform.scale = size;
		return Create(ExStyle, lpClassName, lpWinName, dwStyle, parentsWindow, id, hInstance);
	}

	void SetFont(HFONT font)
	{
		if (font == NULL) return;
		SendMessage(winUI, WM_SETFONT, (WPARAM)font, TRUE);
	}

	LRESULT SendToHWND(UINT message, WPARAM wparam, LPARAM lparam) { return SendMessageW(winUI, message, wparam, lparam); }
	const HWND& GetHWND() const { return winUI; }

	std::wstring GetTextWFromHWND()
	{
		int length = GetWindowTextLengthW(winUI);

		std::wstring text;
		text.resize(length);

		GetWindowTextW(winUI, text.data(), length + 1);
		return text;
	}
	std::string GetTextAFromHWND()
	{
		int length = GetWindowTextLengthA(winUI);

		std::string text;
		text.resize(length);

		GetWindowTextA(winUI, text.data(), length + 1);
		return text;
	}

	bool OnMouse(const Position& mousePos)
	{
		return mousePos.x >= transform.pos.x && mousePos.x <= transform.pos.x + transform.scale.x
			&& mousePos.y >= transform.pos.y && mousePos.y <= transform.pos.y + transform.scale.y;
	}
};



#include <commctrl.h> // ListView 사용
#pragma comment(lib, "Comctl32.lib")
struct CellData
{
	enum_field_types type;
	std::wstring value;
	bool isRealNULL; // 실제 DB의 NULL 여부를 저장하는 플래그

	CellData(enum_field_types type, const std::wstring& value = L"", bool isRealNULL = false) : type(type), value(value), isRealNULL(isRealNULL)
	{}

	bool IsTime()
	{
		return type == MYSQL_TYPE_DATETIME || type == MYSQL_TYPE_TIMESTAMP;
	}

	bool IsNum()
	{
		return type == MYSQL_TYPE_SHORT || type == MYSQL_TYPE_LONG || type == MYSQL_TYPE_LONGLONG || type == MYSQL_TYPE_FLOAT || type == MYSQL_TYPE_DOUBLE || type == MYSQL_TYPE_INT24;
	}
};

class TableUI : public WindowUI
{
	bool useVirtualTable;
	// 헤더나 클래스 멤버로 선언
	std::vector<std::wstring> columns; // 컬럼명 저장
	std::vector<std::vector<CellData>> tableData; // 실제 데이터 저장
public:
	TableUI(const std::wstring& imagePath, const Position& pos, bool useVirtualTable = true) :
		WindowUI(imagePath, pos), useVirtualTable(useVirtualTable)
	{}
	TableUI(const std::wstring& imagePath, const Transform2DINT& transform, bool useVirtualTable = true) :
		WindowUI(imagePath, transform), useVirtualTable(useVirtualTable)
	{}
	~TableUI() = default;

	bool Create(DWORD ExStyle, LPCWSTR lpClassName, LPCWSTR lpWinName, DWORD dwStyle, HWND parentsWindow, HMENU id, HINSTANCE hInstance) override
	{
		bool result = WindowUI::Create(ExStyle, lpClassName, lpWinName, dwStyle, parentsWindow, id, hInstance);

		result = result && SendMessageW(winUI,
			LVM_SETEXTENDEDLISTVIEWSTYLE,
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES,
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES) != NULL;

		return result;
	}
	void Resize()
	{
		//double part = columnSize;
		//int currentWidth = ListView_GetColumnWidth(winUI, 0);
		for (int i = 0; i < columns.size(); i++)
		{
			SendMessage(winUI, LVM_SETCOLUMNWIDTH, i, MAKELPARAM(LVSCW_AUTOSIZE_USEHEADER, 0)); // 헤더 길이에 맞춤
			//SendMessage(winUI, LVM_SETCOLUMNWIDTH, i, MAKELPARAM(LVSCW_AUTOSIZE, 0)); // 데이터 내용에 맞춤
			int currentWidth = ListView_GetColumnWidth(winUI, i);
			ListView_SetColumnWidth(winUI, i, currentWidth + 20);
		}
	}

	
	void SetColumns(const std::vector<std::wstring>& cols);
	void AddRow(const std::vector<CellData>& data);
	void SetItemCount();
	void Clear();

	size_t GetColumnSize() { return columns.size(); }
	size_t GetRowSize() { return tableData.size(); }

	const CellData& GetRealItem(int r, int c) { return tableData[r][c]; }
};

class TreeView : public WindowUI
{
public:
	TreeView(const std::wstring& imagePath, const Position& pos) :
		WindowUI(imagePath, pos)
	{}
	TreeView(const std::wstring& imagePath, const Transform2DINT& transform) :
		WindowUI(imagePath, transform)
	{}

	~TreeView() = default;

	HTREEITEM AddItem(HTREEITEM hParent, const std::wstring& name)
	{
		TVINSERTSTRUCT tvis = { 0 };
		tvis.hParent = hParent;
		tvis.hInsertAfter = TVI_LAST;
		tvis.item.mask = TVIF_TEXT;
		tvis.item.pszText = (LPWSTR)name.c_str();
		return (HTREEITEM)SendMessage(winUI, TVM_INSERTITEM, 0, (LPARAM)&tvis);
	}

	void DeleteAll() { TreeView_DeleteAllItems(winUI); }
};


#include <deque>
/*
	currQuery의 저장 기준을 어떻게 정해야 하는가?
	1. 처음 탐색 시 저장. 이 후 저장하지 않다가 submit시에 다시 초기화.
	2. 탐색하다가 다시 -1 index로 돌아갈 경우마다 저장하기 (채택!)
*/
class HistoryListBox : public WindowUI
{
	int maxSize;
	int currIndex;

	std::wstring currQuery;
	std::deque<std::wstring> historyList;

	void FixIndex()
	{
		if (historyList.empty())
		{
			currIndex = -1;
			return;
		}

		if (currIndex < -1) currIndex = -1;
		else if (currIndex >= (int)historyList.size())
			currIndex = (int)historyList.size() - 1;
	}

public:
	HistoryListBox(const std::wstring& imagePath, const Position& pos, int maxSize = 20) :
		WindowUI(imagePath, pos), maxSize(maxSize), currIndex(-1)
	{}
	HistoryListBox(const std::wstring& imagePath, const Transform2DINT& transform, int maxSize = 20) :
		WindowUI(imagePath, transform), maxSize(maxSize), currIndex(-1)
	{}

	void AddQuery(const std::wstring & sql)
	{
		historyList.push_front(sql);
		//historyList.push_back(sql);
		SendMessage(winUI, LB_INSERTSTRING, 0, (LPARAM)sql.c_str());

		// 너무 많으면 삭제
		if (GetCount() > maxSize) 
		{
			historyList.pop_back();
			//historyList.pop_front();
			SendMessage(winUI, LB_DELETESTRING, GetCount() - 1, 0);
		}
	}

	std::wstring GetSelectedText()
	{
		int idx = SendMessage(winUI, LB_GETCURSEL, 0, 0);
		if (idx == LB_ERR) return L"";

		int len = SendMessage(winUI, LB_GETTEXTLEN, idx, 0);
		std::wstring buf(len, L'\0');
		SendMessage(winUI, LB_GETTEXT, idx, (LPARAM)buf.data());
		return buf;
	}

	std::wstring GetTextFromIndex()
	{
		if (currIndex == -1) return currQuery;
		return historyList[currIndex];
	}

	void TravelIndex(int dir)
	{
		currIndex += dir;
		FixIndex();
	}
	
	void SaveCurrQuery(const std::wstring& query)
	{
		currQuery = query;
	}


	int GetCurrIndex() { return currIndex; }
	int GetCount()  { return historyList.size(); }

};
#endif
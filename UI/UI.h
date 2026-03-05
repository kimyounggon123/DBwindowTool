#ifndef _UI_H
#define _UI_H

#include "..\stdafx.h"
#include "..\Transform.h"
#include <string>

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
class TableUI : public WindowUI
{


	bool useVirtualTable;
	// 헤더나 클래스 멤버로 선언
	std::vector<std::wstring> columns; // 컬럼명 저장
	std::vector<std::vector<std::wstring>> tableData; // 실제 데이터 저장
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
	void Resize(int width)
	{
		//double part = columnSize;
		for (int i = 0; i < columns.size(); i++)
		{
			ListView_SetColumnWidth(winUI, i, width * 0.1);
		}
	}

	void SetColumns(const std::vector<std::wstring>& cols);
	void AddRow(const std::vector<std::wstring>& data);
	void SetItemCount();
	void Clear();

	size_t GetColumnSize() { return columns.size(); }
	size_t GetRowSize() { return tableData.size(); }

	LPWSTR GetRealItem(int r, int c) { return  (LPWSTR)tableData[r][c].c_str(); }
};




#endif
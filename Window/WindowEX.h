#ifndef _DIALOGMAKER_H
#define _DIALOGMAKER_H

#include <Windows.h>
#include <string>
#include "resource.h"
#include "..\UI\UI.h"

struct WindowInformations
{
	HWND hwnd;
	WNDCLASSEXW wc;

	int posX;
	int posY;

	int resolutionX;
	int resolutionY;

	WindowInformations(): hwnd(nullptr), wc{},
		posX(0), posY(0),
		resolutionX(0), resolutionY(0)
	{}


};


class WindowUI : public UIElement
{
protected:
	HWND winUI;
public:
	WindowUI(const std::wstring& imagePath, const Position& pos) :
		UIElement(imagePath, pos), winUI(nullptr)
	{}
	WindowUI(const std::wstring& imagePath, const Transform2DINT& transform) :
		UIElement(imagePath, transform), winUI(nullptr)
	{}

	virtual bool Create(DWORD ExStyle, LPCWSTR lpClassName, LPCWSTR lpWinName, DWORD dwStyle, HWND parentsWindow, HMENU id, HINSTANCE hInstance)
	{
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
};

class WindowEX
{
protected:
	static HFONT hFontNormal;
	static HFONT hFontBold;

	static HINSTANCE hInstance; // program instance handle
	static char szText[256];

	WindowInformations winInfo;

	UImanager& uiManager;

public:
	WindowEX(WindowInformations windowInfo) :
		winInfo(windowInfo), uiManager(UImanager::GetInstance())
	{}
	~WindowEX() {}


	// 대화상자 출력 함수. dialogShapeCode은 실제 rc에 저장된 대화상자 모양 코드번호이고
	// dialogProc은 대화상자를 출력시켜주는 함수 포인터이다.
	static void InitializeParam(HINSTANCE hInstance);
	virtual bool InitializeWindow(const wchar_t* title, WNDPROC wndProc);
	void show(int cmdSize);
	virtual void Shutdown();

	// 원하는 대화상자를 제작하는 클래스 (static으로 제작)
	static LRESULT CALLBACK MainProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif

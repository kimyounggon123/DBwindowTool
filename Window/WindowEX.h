#ifndef _DIALOGMAKER_H
#define _DIALOGMAKER_H

#include <Windows.h>
#include <string>
#include "resource.h"

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

class WindowEX
{
	static HINSTANCE hInstance; // program instance handle
	static char szText[256];

	WindowInformations winInfo;

public:
	WindowEX(WindowInformations windowInfo) :winInfo(windowInfo)
	{}
	~WindowEX() {}


	// 대화상자 출력 함수. dialogShapeCode은 실제 rc에 저장된 대화상자 모양 코드번호이고
	// dialogProc은 대화상자를 출력시켜주는 함수 포인터이다.
	static void InitializeParam(HINSTANCE hInstance);
	bool InitializeWindow(const wchar_t* title, WNDPROC wndProc);
	void show(int cmdSize);


	// 원하는 대화상자를 제작하는 클래스 (static으로 제작)
	static LRESULT CALLBACK MainProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif

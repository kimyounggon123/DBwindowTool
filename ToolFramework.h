#ifndef _TOOLFRAMEWORK_H
#define _TOOLFRAMEWORK_H

#include "stdafx.h"
#include "WindowEX.h"
#include "DatabaseWindow.h"
#include "UI/UI.h"

class ToolFramework
{
	int cmdShow;
	UImanager& uiManager;
	WindowEX* window;

	static ToolFramework* instance;
	ToolFramework() :
		cmdShow(0),
		uiManager(UImanager::GetInstance()),
		window(nullptr)
	{}

public:
	static ToolFramework& GetInstance()
	{
		if (instance == nullptr) instance = new ToolFramework();
		return *instance;
	}

	~ToolFramework()
	{
		SAFE_FREE(window);
	}


	bool Initialize(const wchar_t* title, WNDPROC wndproc, const Vector2Int& pos, const Vector2Int& resolution,  HINSTANCE hInstance, int cmdShow);
	void Run();
	void Close();
};

#endif
#include "ToolFramework.h"
ToolFramework* ToolFramework::instance = nullptr;



bool ToolFramework::Initialize(const wchar_t* title, WNDPROC wndproc, const Vector2Int& pos, const Vector2Int& resolution,  HINSTANCE hInstance, int cmdShow)
{
	if (title == nullptr || hInstance == NULL) return false;
	WindowEX::InitializeParam(hInstance);

	WindowInformations info;
	info.posX = pos.x; info.posY = pos.y;
	info.resolutionX = resolution.x; info.resolutionY = resolution.y;

	window = new DatabaseWindow(info, false);
	this->cmdShow = cmdShow;
	window->InitializeWindow(title, wndproc);

	uiManager.Initialize();

	return true;
}


void ToolFramework::Run()
{
	window->show(cmdShow);
}
void ToolFramework::Close()
{
	uiManager.DeleteAll();
	window->Shutdown();
}
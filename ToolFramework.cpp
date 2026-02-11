#include "ToolFramework.h"
ToolFramework* ToolFramework::instance = nullptr;



bool ToolFramework::Initialize(const wchar_t* title, WNDPROC wndproc, const Vector2Int& pos, const Vector2Int& resolution,  HINSTANCE hInstance, int cmdShow)
{
	if (title == nullptr || hInstance == NULL) return false;
	WindowEX::InitializeParam(hInstance);

	WindowInformations info;
	info.posX = pos.x; info.posY = pos.y;
	info.resolutionX = resolution.x; info.resolutionY = resolution.y;

	window = new WindowEX(info);
	this->cmdShow = cmdShow;
	window->InitializeWindow(title, wndproc);

	return true;
}

bool ToolFramework::UpdateUI()
{
	uiManager.AddUI(L"", Transform2DINT{ {0, 0}, {1.0f, 1.0f}, 0.0f }, RectINT{ 0, 0, 0, 0 });

	return true;
}
void ToolFramework::Run()
{
	window->show(cmdShow);
}
void ToolFramework::Close()
{
	uiManager.DeleteAll();
}
#include "WindowEX.h"
#include "ToolFramework.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	ToolFramework& frame = ToolFramework::GetInstance();

	frame.Initialize(
		L"hello",
		WindowEX::MainProc,
		{0, 0},
		{1280, 840},
		hInstance,
		nCmdShow);

	frame.Run();
	frame.Close();
	return 0;
}

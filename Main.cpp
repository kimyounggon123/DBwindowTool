#include "DataBaseWindow.h"
#include "ToolFramework.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	ToolFramework& frame = ToolFramework::GetInstance();

	frame.Initialize(
		L"DB tool",
		DatabaseWindow::DBMain,
		{0, 0},
		{1440, 800},
		hInstance,
		nCmdShow);

	frame.Run();
	frame.Close();
	return 0;
}

#include "DatabaseWindow.h"
bool DatabaseWindow::InitializeWindow(const wchar_t* title, WNDPROC wndProc)
{
    WindowEX::InitializeWindow(title, wndProc);
    editUI = new WindowUI(L"", { {0, 0}, {100, 100} , 0.0f });
}

LRESULT CALLBACK DatabaseWindow::DBMain(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HWND hEdit;
    static HWND hButton;

    switch (msg)
    {
    case WM_CREATE:
    {
        hEdit = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            L"EDIT",
            L"",
            WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
            10, 10,
            600, 150,
            hwnd,
            (HMENU)1001,
            WindowEX::hInstance,
            nullptr
        );

        hButton = CreateWindow(
            L"BUTTON",
            L"Execute",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            620, 10,
            120, 40,
            hwnd,
            (HMENU)1002,
            WindowEX::hInstance,
            nullptr
        );
        break;
    }
        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
            case 1002: // Execute 버튼
            {
                wchar_t buffer[4096];
                GetWindowText(hEdit, buffer, 4096);

                std::wstring query = buffer;

                //ExecuteQuery(query);  // 여기서 DB 실행
                break;
            }
            }

            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}
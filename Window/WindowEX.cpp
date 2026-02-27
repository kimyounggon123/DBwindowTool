#include "WindowEX.h"

HINSTANCE WindowEX::hInstance = NULL;
HFONT WindowEX::hFontNormal = NULL;
HFONT WindowEX::hFontBold = NULL;


char WindowEX::szText[256] = { 0 };

std::wstring WindowEX::GetTimeString()
{
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);

    std::tm tm{};
    localtime_s(&tm, &t);   // Windows 안전 버전

    std::wstringstream ss;
    ss << L"["
        << std::setw(2) << std::setfill(L'0') << tm.tm_hour << L":"
        << std::setw(2) << std::setfill(L'0') << tm.tm_min << L":"
        << std::setw(2) << std::setfill(L'0') << tm.tm_sec
        << L"]";

    return ss.str();
}
std::wstring WindowEX::GetTimeStringWin32()
{
    SYSTEMTIME st;
    GetLocalTime(&st);

    wchar_t buffer[16];
    swprintf_s(buffer, L"[%02d:%02d:%02d]",
        st.wHour, st.wMinute, st.wSecond);

    return buffer;
}


void WindowEX::InitializeParam(HINSTANCE hInstance) { WindowEX::hInstance = hInstance; }
bool WindowEX::InitializeWindow(const wchar_t* title, WNDPROC wndProc)
{
    WNDCLASSEXW& wc = winInfo.wc;
    HWND& hwnd = winInfo.hwnd;

    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = wndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MyWindowClass";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassExW(&wc);

    hFontNormal = CreateFontW(
        18,                 // 높이
        0, 0, 0,
        FW_NORMAL,          // 폰트
        FALSE, FALSE, FALSE, // 이탤릭, 언더바, 크로스바
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        L"맑은 고딕"
    );

    hFontBold = CreateFontW(
        18,                 // 높이
        0, 0, 0,
        FW_BOLD,          // 폰트
        FALSE, FALSE, FALSE, // 이탤릭, 언더바, 크로스바
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        L"맑은 고딕"
    );

    hwnd = CreateWindowExW(
        0,                      // ex style
        wc.lpszClassName,             // class name
        title,         // window title
        WS_OVERLAPPEDWINDOW,    // style
        winInfo.posX, winInfo.posY,
        winInfo.resolutionX, winInfo.resolutionY,
        nullptr,                // parent (버튼 등의 ui는 부모를 base window로 설정. base window는 nullptr로)
        nullptr,                // menu (버튼 스타일 정의. base window는 nullptr로)
        hInstance,
        this                 // lpParam (WM_NCCREATE/WM_CREATE로 전달됨)
    );

    uiManager.Initialize();

    return hwnd != NULL;
}
LRESULT CALLBACK WindowEX::MainProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HWND hBtnLogin;
    /*
        L"BUTTON" : 버튼

        L"EDIT" : 에디트 박스

        L"STATIC" : 텍스트 라벨

        L"LISTBOX" : 리스트박스

        L"COMBOBOX" : 콤보박스

        L"SCROLLBAR" : 스크롤바

        이건 <commctrl.h> + InitCommonControlsEx()가 필요한 컨트롤들.

        WC_LISTVIEW 또는 L"SysListView32" : 리스트뷰

        WC_TREEVIEW 또는 L"SysTreeView32" : 트리뷰

        WC_TABCONTROL 또는 L"SysTabControl32" : 탭

        PROGRESS_CLASS 또는 L"msctls_progress32" : 프로그레스바
    */

    /*
    * 4번 인자: UI 스타일
        BS_DEFPUSHBUTTON : 기본 버튼(Enter 누르면 눌리는 버튼)

        BS_CHECKBOX : 체크박스

        BS_AUTOCHECKBOX : 클릭하면 자동으로 체크 on/off 토글

        BS_RADIOBUTTON : 라디오 버튼

        BS_AUTORADIOBUTTON : 자동 토글되는 라디오 버튼

        BS_GROUPBOX : 그룹 박스(테두리 박스)
    
    */

    switch (msg)
    {
    case WM_CREATE:
        hBtnLogin = CreateWindowExW(
            0,
            L"STATIC", // 2번째 
            L"Login",
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            20, 20, 120, 35,
            hwnd,
            (HMENU)1001,                 // 버튼 ID
            WindowEX::hInstance,
            nullptr
        );
        return 0;


    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case 1001: // 버튼 ID
            //MessageBoxW(hwnd, L"Button Click!", L"Info", MB_OK);
            return 0;
        }
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}



void WindowEX::show(int cmdSize)
{
    // 창을 화면에 보여 줌
    ShowWindow(winInfo.hwnd, cmdSize); // nCmdShow : 첫 창의 크기를 설정. os에서 받아 오기
    UpdateWindow(winInfo.hwnd);         // 즉시 WM_PAINT(그리기)를 유도하는 함수. ShowWindow만 해도 되나 그리기르르 촉진시킴.
 
    //while (GetMessage(&msg, nullptr, 0, 0))
    //{
    //    TranslateMessage(&msg);
    //    DispatchMessage(&msg);
    //}

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!IsDialogMessage(winInfo.hwnd, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}
void WindowEX::Shutdown()
{

}
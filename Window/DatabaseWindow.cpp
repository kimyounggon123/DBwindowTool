#include "DatabaseWindow.h"

DatabaseAccount* DatabaseWindow::account = nullptr;
DBQueryExamples DatabaseWindow::queryExample;

// 여러 ui들
WindowUI* id = nullptr;
WindowUI* pw = nullptr;

WindowUI* logInBtn = nullptr;
WindowUI* logOutBtn = nullptr;

WindowUI* currIDTag = nullptr;
WindowUI* currDatabaseTag = nullptr;
WindowUI* currID = nullptr;
WindowUI* currDatabase = nullptr;

WindowUI* editUI = nullptr;
WindowUI* resultLog = nullptr;
WindowUI* submitButton = nullptr;

TableUI* listView = nullptr;


WindowUI* selectQueryBtn = nullptr;
WindowUI* createDBQueryBtn = nullptr;


bool DatabaseWindow::InitializeWindow(const wchar_t* title, WNDPROC wndProc)
{
    account = new DatabaseAccount();
    InitializeUI();

    WindowEX::InitializeWindow(title, wndProc);
    return true;
}

void DatabaseWindow::InitializeUI()
{
    icex.dwSize = sizeof(icex);
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    // EDIT
    /*

    */

    // 현재 로그인한 정보
    std::unique_ptr<WindowUI> currIDTagptr = std::make_unique<WindowUI>
        (L"", Transform2DINT({ Position{640, 10}, Vector2Int{80, 20} , 0.0f }));
    currIDTag = currIDTagptr.get();
    uiManager.AddUI(std::move(currIDTagptr));

    std::unique_ptr<WindowUI> currDBTagptr = std::make_unique<WindowUI>
        (L"", Transform2DINT({ Position{640, 32}, Vector2Int{80, 20} , 0.0f }));
    currDatabaseTag = currDBTagptr.get();
    uiManager.AddUI(std::move(currDBTagptr));

    std::unique_ptr<WindowUI> currIDptr = std::make_unique<WindowUI>
        (L"", Transform2DINT({ Position{720, 10}, Vector2Int{220, 20} , 0.0f }));
    currID = currIDptr.get();
    uiManager.AddUI(std::move(currIDptr));

    std::unique_ptr<WindowUI> currDBptr = std::make_unique<WindowUI>
        (L"", Transform2DINT({ Position{720, 32}, Vector2Int{220, 20} , 0.0f }));
    currDatabase = currDBptr.get();
    uiManager.AddUI(std::move(currDBptr));


    // 로그인 영역
    std::unique_ptr<WindowUI> idInputptr = std::make_unique<WindowUI>
        (L"", Transform2DINT({ Position{960, 10}, Vector2Int{300, 20} , 0.0f }));
    id = idInputptr.get();
    uiManager.AddUI(std::move(idInputptr));

    std::unique_ptr<WindowUI> pwInputptr = std::make_unique<WindowUI>
        (L"", Transform2DINT({ Position{960, 32}, Vector2Int{300, 20} , 0.0f }));
    pw = pwInputptr.get();
    uiManager.AddUI(std::move(pwInputptr));

    std::unique_ptr<WindowUI> logInBtnptr = std::make_unique<WindowUI>
        (L"", Transform2DINT({ Position{1270, 10}, Vector2Int{60, 44} , 0.0f }));
    logInBtn = logInBtnptr.get();
    uiManager.AddUI(std::move(logInBtnptr));

    std::unique_ptr<WindowUI> logOutBtnptr = std::make_unique<WindowUI>
        (L"", Transform2DINT({ Position{1340, 10}, Vector2Int{60, 44} , 0.0f }));
    logOutBtn = logOutBtnptr.get();
    uiManager.AddUI(std::move(logOutBtnptr));


    // 입력창
    std::unique_ptr<WindowUI> result = std::make_unique<WindowUI>
        (L"", Transform2DINT({ Position{640, 80}, Vector2Int{768, 210} , 0.0f }));
    resultLog = result.get();
    uiManager.AddUI(std::move(result));

    std::unique_ptr<WindowUI> edit = std::make_unique<WindowUI>
        (L"", Transform2DINT({ Position{640, 300}, Vector2Int{768, 210} , 0.0f }));
    editUI = edit.get();
    uiManager.AddUI(std::move(edit));

    std::unique_ptr<WindowUI> submit = std::make_unique<WindowUI>
        (L"", Transform2DINT({ Position{640, 520}, Vector2Int{768, 30} , 0.0f }));
    submitButton = submit.get();
    uiManager.AddUI(std::move(submit));



    std::unique_ptr<TableUI> listViewUI = std::make_unique<TableUI>
        (L"", Transform2DINT({ Position{10, 10}, Vector2Int{620, 740} , 0.0f }));
    listView = listViewUI.get();
    uiManager.AddUI(std::move(listViewUI));

}

void DatabaseWindow::Shutdown()
{
    SAFE_FREE(account);
}

LRESULT CALLBACK DatabaseWindow::DBMain(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UImanager& ui = UImanager::GetInstance();
    switch (msg)
    {
    case WM_CREATE:
        WM_CREATE_FUNC(hwnd, msg, wParam, lParam);
        break;
    

    case WM_SIZE:
    {
        int width = LOWORD(lParam);
        listView->Resize(width);
        break;
    }

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case 1003: // Execute 버튼     
            SendQuery(hwnd, msg, wParam, lParam);
            break;
        
        case 1100:
            LogIn(hwnd, msg, wParam, lParam);
            break;
        case 1101:
            LogOut(hwnd, msg, wParam, lParam);
            break;
        
        }
        break;
    }

    case WM_CTLCOLORSTATIC:
    {
        /*
        HDC hdcStatic = (HDC)wParam;
        HWND hStatic = (HWND)lParam;

        if (GetDlgCtrlID(hStatic) == 1005)
        {
            SetBkMode(hdcStatic, TRANSPARENT);
            return (LRESULT)GetStockObject(NULL_BRUSH);
        }
        */
        break;
    }
    
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void DatabaseWindow::WM_CREATE_FUNC(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  
    id->Create(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        hwnd,
        nullptr,
        WindowEX::hInstance
    );
    id->SetFont(hFontNormal);

    pw->Create(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_PASSWORD,
        hwnd,
        nullptr,
        WindowEX::hInstance
    );
    pw->SetFont(hFontNormal);

    logInBtn->Create(
        0,
        L"BUTTON",
        L"Log In",
        WS_CHILD | WS_VISIBLE |  BS_PUSHBUTTON,
        hwnd,             // 부모
        (HMENU)1100,      // ID
        hInstance
    );
    logInBtn->SetFont(hFontBold);

    logOutBtn->Create(
        0,
        L"BUTTON",
        L"Log Out",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        hwnd,             // 부모
        (HMENU)1101,      // ID
        hInstance
    );
    logOutBtn->SetFont(hFontBold);

    currIDTag->Create(
        0,
        L"STATIC",
        L"User ID:",
        WS_CHILD | WS_VISIBLE,
        hwnd,
        nullptr,
        WindowEX::hInstance
    );
    currIDTag->SetFont(hFontBold);


    currDatabaseTag->Create(
        0,
        L"STATIC",
        L"Database:",
        WS_CHILD | WS_VISIBLE,
        hwnd,
        nullptr,
        WindowEX::hInstance
    );
    currDatabaseTag->SetFont(hFontBold);

    currID->Create(
        0,
        L"STATIC",
        L"",
        WS_CHILD | WS_VISIBLE,
        hwnd,
        nullptr,
        WindowEX::hInstance
    );
    currID->SetFont(hFontBold);
  
    currDatabase->Create(
        0,
        L"STATIC",
        L"",
        WS_CHILD | WS_VISIBLE,
        hwnd,
        nullptr,
        WindowEX::hInstance
    );
    currDatabase->SetFont(hFontBold);

    editUI->Create(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | WS_VSCROLL | WS_TABSTOP,
        hwnd,
        (HMENU)1001,
        WindowEX::hInstance
    );
    editUI->SetFont(hFontNormal);

    submitButton->Create(
        0,
        L"BUTTON",
        L"Submit",
        WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP,
        hwnd,             // 부모
        (HMENU)1003,      // ID
        hInstance
    );
    submitButton->SetFont(hFontBold);

    resultLog->Create(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL | ES_READONLY,
        hwnd,
        (HMENU)1005,
        WindowEX::hInstance
    );
    resultLog->SetFont(hFontNormal);

    listView->Create(
        WS_EX_CLIENTEDGE,
        WC_LISTVIEW,               // 핵심
        L"",
        WS_CHILD | WS_VISIBLE | ES_READONLY | LVS_REPORT | LVS_SINGLESEL,            // 테이블 모드
        hwnd,
        (HMENU)3001,
        hInstance
    );
    
}

void DatabaseWindow::LogIn(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    std::wstring idText = id->GetTextWFromHWND();
    std::wstring pwText = pw->GetTextWFromHWND();

    bool result = account->Connect("localhost", account->WStringToUTF8(idText).c_str(), account->WStringToUTF8(pwText), "None");
    if (result == true)
    {
        currID->SendToHWND(WM_SETTEXT, 0, (LPARAM)idText.c_str());
        currDatabase->SendToHWND(WM_SETTEXT, 0, (LPARAM)account->UTF8ToWString("None").c_str());
        ShowResultMsg(L"Log In success", false);
    }
    else
    {
        ShowResultMsg(account->GetLastErrorW(), false);
    }
    id->SendToHWND(WM_SETTEXT, 0, (LPARAM)L""); // 문자 초기화
    pw->SendToHWND(WM_SETTEXT, 0, (LPARAM)L""); // 문자 초기화
}

void DatabaseWindow::LogOut(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (!account->IsConnected())
    {
        ShowResultMsg(L"No connection", false);
        return;
    }
    account->Close();
    currID->SendToHWND(WM_SETTEXT, 0, (LPARAM)L"");
    currDatabase->SendToHWND(WM_SETTEXT, 0, (LPARAM)L"None");
    ShowResultMsg(L"Log Out", false);
}



void DatabaseWindow::SanitizeForListView(std::wstring& str)
{
    str.erase(
        std::remove_if(str.begin(), str.end(),
            [](wchar_t c)
            {
                return c == L'\r' || c == L'\n';
            }),
        str.end()    
        );
}

void DatabaseWindow::SendQuery(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    std::wstring query = editUI->GetTextWFromHWND();
    //SanitizeForListView(query); // 개행 문자 \n \r 삭제
    if (!account->ExecuteQuery(account->WStringToUTF8(query)))
    {
        ShowResultMsg(account->GetLastErrorW(), false);
        return;
    }

    listView->Clear();

    MYSQL_FIELD* fields = account->getFieldName();
    unsigned int num_fields = account->getFieldNum();
    MYSQL_ROW row = nullptr;

    std::vector<std::wstring> columns;
    for (unsigned int i = 0; i < num_fields; ++i)  // 각 row의 field를 출력
        columns.push_back(account->UTF8ToWString(fields[i].name));
    listView->SetColumns(columns);

    while ((row = account->fetchRow()))
    {
        std::vector<std::wstring> oneRow;

        // 각 필드는 문자열(char*) 형태로 반환됨
        for (unsigned int i = 0; i < num_fields; ++i)  // 각 row의 field를 출력
        {
            if (row[i])
                oneRow.push_back(account->UTF8ToWString(row[i]));
            else
                oneRow.push_back(L"NULL");
        }
        listView->AddRow(oneRow);
    }
    
    ShowResultMsg(L"Used query : " + query, false);
    editUI->SendToHWND(WM_SETTEXT, 0, (LPARAM)L""); // 문자 초기화
    currDatabase->SendToHWND(WM_SETTEXT, 0, (LPARAM)account->GetDatabaseName().c_str());
}


void DatabaseWindow::ShowResultMsg(const std::wstring& str, bool isError)
{
    std::wstring time = GetTimeString();
    std::wstring final = time + L" " + str + L"\r\n";

    int length = GetWindowTextLengthW(resultLog->GetHWND());
    resultLog->SendToHWND(EM_SETSEL, length, length);
    resultLog->SendToHWND(EM_REPLACESEL, FALSE, (LPARAM)final.c_str());
}
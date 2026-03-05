#include "DatabaseWindow.h"

DatabaseAccount* DatabaseWindow::account = nullptr;
DBQueryExamples DatabaseWindow::queryExample;

// 여러 ui들
WindowUI* id = nullptr;
WindowUI* pw = nullptr;

Button* logInBtn = nullptr;
Button* logOutBtn = nullptr;

WindowUI* currIDTag = nullptr;
WindowUI* currDatabaseTag = nullptr;
WindowUI* currID = nullptr;
WindowUI* currDatabase = nullptr;

WindowUI* editUI = nullptr;
WindowUI* resultLog = nullptr;

Button* submitButton = nullptr;

TableUI* listView = nullptr;

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

    // 현재 로그인한 정보
    std::unique_ptr<WindowUI> currIDTagptr = std::make_unique<WindowUI>
        (L"", Transform2DINT({ Position{640, 10}, Vector2Int{80, 20}}));
    currIDTag = currIDTagptr.get();
    uiManager.AddUI(std::move(currIDTagptr));

    std::unique_ptr<WindowUI> currDBTagptr = std::make_unique<WindowUI>
        (L"", Transform2DINT({ Position{640, 32}, Vector2Int{80, 20}}));
    currDatabaseTag = currDBTagptr.get();
    uiManager.AddUI(std::move(currDBTagptr));

    std::unique_ptr<WindowUI> currIDptr = std::make_unique<WindowUI>
        (L"", Transform2DINT({ Position{720, 10}, Vector2Int{220, 20}}));
    currID = currIDptr.get();
    uiManager.AddUI(std::move(currIDptr));

    std::unique_ptr<WindowUI> currDBptr = std::make_unique<WindowUI>
        (L"", Transform2DINT({ Position{720, 32}, Vector2Int{220, 20} }));
    currDatabase = currDBptr.get();
    uiManager.AddUI(std::move(currDBptr));


    // 로그인 영역
    std::unique_ptr<WindowUI> idInputptr = std::make_unique<WindowUI>
        (L"", Transform2DINT({ Position{960, 10}, Vector2Int{300, 20} }));
    id = idInputptr.get();
    uiManager.AddUI(std::move(idInputptr));

    std::unique_ptr<WindowUI> pwInputptr = std::make_unique<WindowUI>
        (L"", Transform2DINT({ Position{960, 32}, Vector2Int{300, 20}}));
    pw = pwInputptr.get();
    uiManager.AddUI(std::move(pwInputptr));

    std::unique_ptr<Button> logInBtnptr = std::make_unique<Button>
        (L"", Transform2DINT({ Position{1270, 10}, Vector2Int{60, 44} }));
    logInBtn = logInBtnptr.get();
    uiManager.AddUI(std::move(logInBtnptr));

    std::unique_ptr<Button> logOutBtnptr = std::make_unique<Button>
        (L"", Transform2DINT({ Position{1340, 10}, Vector2Int{60, 44}}));
    logOutBtn = logOutBtnptr.get();
    uiManager.AddUI(std::move(logOutBtnptr));


    // 입력창
    std::unique_ptr<WindowUI> result = std::make_unique<WindowUI>
        (L"", Transform2DINT({ Position{640, 70}, Vector2Int{768, 310}}));
    resultLog = result.get();
    uiManager.AddUI(std::move(result));

    std::unique_ptr<WindowUI> edit = std::make_unique<WindowUI>
        (L"", Transform2DINT({ Position{640, 390}, Vector2Int{768, 310} }));
    editUI = edit.get();
    uiManager.AddUI(std::move(edit));

    std::unique_ptr<Button> submit = std::make_unique<Button>
        (L"", Transform2DINT({ Position{640, 710}, Vector2Int{768, 40}}));
    submitButton = submit.get();
    uiManager.AddUI(std::move(submit));


    // 테이블
    std::unique_ptr<TableUI> listViewUI = std::make_unique<TableUI>
        (L"", Transform2DINT({ Position{10, 10}, Vector2Int{620, 740} }));
    listView = listViewUI.get();
    uiManager.AddUI(std::move(listViewUI));
}

void DatabaseWindow::Shutdown()
{
    SAFE_FREE(account);
}


LRESULT CALLBACK DatabaseWindow::DBLogIn(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UImanager& ui = UImanager::GetInstance();
    switch (msg)
    {
    case WM_CREATE:
        WM_CREATE_FUNC(hwnd, msg, wParam, lParam);
        break;

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case 1100:
            LogIn(hwnd, msg, wParam, lParam);
            break;
        case 1101:
            LogOut(hwnd, msg, wParam, lParam);
            break;
        }
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
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
    case WM_NOTIFY:
    {
        NotifyTable(hwnd, msg, wParam, lParam);
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
    // id 창
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


    // query 입력 창
    editUI->Create(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | WS_VSCROLL | WS_TABSTOP | ES_MULTILINE | ES_WANTRETURN,
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

    // 결과 및 에러 출력 창
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

    // table
    listView->Create(
        WS_EX_CLIENTEDGE,
        WC_LISTVIEW,               // 테이블 모드 사용 시 해당 코드 입력
        L"",
        WS_CHILD | WS_VISIBLE | ES_READONLY | LVS_REPORT | LVS_SINGLESEL | LVS_OWNERDATA,            // 테이블 모드
        hwnd,
        (HMENU)3001,
        hInstance
    );
    listView->SendToHWND((0x1000 + 47), (WPARAM)(10000), 0); //  ListView_SetItemCount(hList, 10000);
    
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


void DatabaseWindow::SendQuery(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    std::wstring query = editUI->GetTextWFromHWND();
    try
    {
        if (!account->ExecuteQuery(account->WStringToUTF8(query))) throw false;
        
        listView->Clear(); // 성공 시에만 기존 테이블 제거

        MYSQL_FIELD* fields = account->getFieldName();
        unsigned int num_fields = account->getFieldNum();
        

        std::vector<std::wstring> columns;
        for (unsigned int i = 0; i < num_fields; ++i)  // 각 row의 field를 출력
            columns.push_back(account->UTF8ToWString(fields[i].name));
        listView->SetColumns(columns);

        MYSQL_ROW row = nullptr;
        while ((row = account->fetchRow()))
        {
            std::vector<std::wstring> oneRow;

            // 각 필드는 문자열(char*) 형태로 반환됨
            for (unsigned int i = 0; i < num_fields; ++i)  // 각 row의 field를 출력
            {
                if (row[i]) oneRow.push_back(account->UTF8ToWString(row[i]));
                else oneRow.push_back(L"NULL");
            }
            listView->AddRow(oneRow);
        }
        listView->SetItemCount();

        ShowResultMsg(L"Used query : " + query, false);
        currDatabase->SendToHWND(WM_SETTEXT, 0, (LPARAM)account->GetDatabaseName().c_str());
    }
    catch (...)
    {
        ShowResultMsg(account->GetLastErrorW(), false);
    }
   
    editUI->SendToHWND(WM_SETTEXT, 0, (LPARAM)L""); // 문자 초기화
}


void DatabaseWindow::NotifyTable(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LPNMHDR pnmh = (LPNMHDR)lParam;
    if (pnmh->code == LVN_GETDISPINFO) {
        NMLVDISPINFO* plvdi = (NMLVDISPINFO*)lParam;

        if (plvdi->item.mask & LVIF_TEXT) {
            int r = plvdi->item.iItem;      // 행 번호
            int c = plvdi->item.iSubItem;   // 열 번호

            // 안전장치: 벡터 범위를 벗어나지 않는지 확인
           // 1. 행 인덱스 검사
            if (r >= listView->GetRowSize()) return;

            // 2. 열 인덱스 검사
            if (c >= listView->GetColumnSize()) {
                plvdi->item.pszText = (LPWSTR)L"NULL"; // 데이터가 없으면 null
                return;
            }


            // 3. 실제 데이터 연결
            plvdi->item.pszText = listView->GetRealItem(r, c);
        }
    }
}
void DatabaseWindow::StartTransaction()
{
    bool result = account->StartTransaction();
    std::wstring resultLog = result == true ? L"Start Transaction;" : account->GetLastErrorW();
    ShowResultMsg(resultLog);
}
void DatabaseWindow::Commit()
{
    bool result = account->Commit();
    std::wstring resultLog = result == true ? L"Commit;" : account->GetLastErrorW();
    ShowResultMsg(resultLog);
}
void DatabaseWindow::Rollback()
{
    bool result = account->Rollback();
    std::wstring resultLog = result == true ? L"Rollback;" : account->GetLastErrorW();
    ShowResultMsg(resultLog);
}

void DatabaseWindow::ShowResultMsg(const std::wstring& str, bool isError)
{
    std::wstring time = GetTimeString();
    std::wstring final = time + L" " + str + L"\r\n";

    int length = GetWindowTextLengthW(resultLog->GetHWND());
    resultLog->SendToHWND(EM_SETSEL, length, length);
    resultLog->SendToHWND(EM_REPLACESEL, FALSE, (LPARAM)final.c_str());
}
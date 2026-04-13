#include "DatabaseWindow.h"

DatabaseAccount* DatabaseWindow::account = nullptr;
DBQueryExamples DatabaseWindow::queryExample;

HMENU DatabaseWindow::hMenuBar = NULL;


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
Button* submitButton = nullptr;
Button* refreshButton = nullptr;

Toggle* autoCommitToggle = nullptr;
Button* commitButton = nullptr;
Button* rollbackButton = nullptr;

WindowUI* resultLog = nullptr;
Button* resultClearButton = nullptr;
WindowUI* usedTime = nullptr;
WindowUI* numOfColumns = nullptr;


TableUI* listView = nullptr;
TreeView* hTreeView = nullptr;
HistoryListBox* prevQueryListBox = nullptr;

CHARFORMAT2 g_defaultCF;
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
        //int width = LOWORD(lParam);
        listView->Resize();
        break;
    }
    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lParam;
        HDC hdc = lpDrawItem->hDC;
        RECT rect = lpDrawItem->rcItem;

        DrawAutoTransaction(lpDrawItem, hdc, rect);

        return TRUE;
    }

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ID_LOGIN:
            LogIn(hwnd, msg, wParam, lParam);
            break;
        case ID_LOGOUT:
            LogOut(hwnd, msg, wParam, lParam);
            break;

        case ID_SUBMIT: // Execute 버튼     
            SendQuery(hwnd, msg, wParam, lParam);
            break;


        case ID_REFRESH:
            RefreshAll();
            break;

        case ID_PREV_QUERYLIST:
            editUI->SendToHWND(WM_SETTEXT, 0, (LPARAM)prevQueryListBox->GetSelectedText().c_str());
            break;

        case ID_AUTOCOMMIT:
            SetTransactionMode(hwnd, msg, wParam, lParam);
            autoCommitToggle->Invalidate();

            break;

        case ID_COMMIT:
            SetTransactionMode(TransactionType::Commit);
            autoCommitToggle->Invalidate();
            commitButton->Invalidate();
            rollbackButton->Invalidate();
            break;

        case ID_ROLLBACK:
            SetTransactionMode(TransactionType::Rollback);
            autoCommitToggle->Invalidate();
            commitButton->Invalidate();
            rollbackButton->Invalidate();
            break;

        case ID_CLEAR_RESULT:
            resultLog->SendToHWND(WM_SETTEXT, 0, (LPARAM)L""); // 문자 초기화
            break;

        case ID_EDIT:
        {
            if (HIWORD(wParam) == EN_CHANGE)
            {
                static bool isFormatting = false;
                if (!isFormatting)
                {
                    isFormatting = true;
                    ApplySqlHighlight();
                    isFormatting = false;
                }
            }
            break;
        }

        case ID_MENU_LOAD_QUERY:
            // 파일 열기 다이얼로그 띄우는 함수 호출
            //LoadQueryFromFile();
            break;

        case ID_MENU_SAVE_QUERY:
            // 현재 에디트 박스 텍스트 파일로 저장
            //SaveQueryToFile();
            break;

        case ID_MENU_EXIT:
            DestroyWindow(hwnd);
            break;

        }
        break;
    }
    case WM_NOTIFY:
    {
        NotifyTreeClick(hwnd, msg, wParam, lParam);
        NotifyTableMaking(hwnd, msg, wParam, lParam);
        return NotifyTableColoring(hwnd, msg, wParam, lParam);
        break;
    }

    case WM_CTLCOLORSTATIC:
    {
        HDC hdcStatic = (HDC)wParam;
        HWND hwndStatic = (HWND)lParam;


        if (GetDlgCtrlID(hwndStatic) == ID_USED_TIME || GetDlgCtrlID(hwndStatic) == ID_NUMOF_COLUMNS)
        {
            // 글자 뒤의 배경색을 흰색으로 설정
            SetBkColor(hdcStatic, RGB(255, 255, 255));

            // 글자 자체의 색상을 설정 (필요한 경우)
            // SetTextColor(hdcStatic, RGB(0, 0, 0));

            // 컨트롤의 나머지 영역을 채울 흰색 브러시 반환
            return (INT_PTR)GetStockObject(WHITE_BRUSH);
        }
        //return (INT_PTR)GetSysColorBrush(COLOR_BTNFACE); // 나머지는 기본색 유지

        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}


//////////////////////////////////////////////////////////// Initialize ////////////////////////////////////////////////////////////
bool DatabaseWindow::InitializeWindow(const wchar_t* title, WNDPROC wndProc)
{
    account = new DatabaseAccount();
    
    icex.dwSize = sizeof(icex);
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    // 현재 로그인한 정보
    std::unique_ptr<WindowUI> currIDTagptr = std::make_unique<WindowUI>(L"", Transform2DINT({ Position{640, 10}, Vector2Int{80, 20} }));
    currIDTag = currIDTagptr.get();
    uiManager.AddUI(std::move(currIDTagptr));

    std::unique_ptr<WindowUI> currDBTagptr = std::make_unique<WindowUI>(L"", Transform2DINT({ Position{640, 32}, Vector2Int{80, 20} }));
    currDatabaseTag = currDBTagptr.get();
    uiManager.AddUI(std::move(currDBTagptr));

    std::unique_ptr<WindowUI> currIDptr = std::make_unique<WindowUI>(L"", Transform2DINT({ Position{720, 10}, Vector2Int{210, 20} }));
    currID = currIDptr.get();
    uiManager.AddUI(std::move(currIDptr));

    std::unique_ptr<WindowUI> currDBptr = std::make_unique<WindowUI>(L"", Transform2DINT({ Position{720, 32}, Vector2Int{210, 20} }));
    currDatabase = currDBptr.get();
    uiManager.AddUI(std::move(currDBptr));


    // 로그인 영역
    std::unique_ptr<WindowUI> idInputptr = std::make_unique<WindowUI>(L"", Transform2DINT({ Position{940, 10}, Vector2Int{300, 20} }));
    id = idInputptr.get();
    uiManager.AddUI(std::move(idInputptr));

    std::unique_ptr<WindowUI> pwInputptr = std::make_unique<WindowUI>(L"", Transform2DINT({ Position{940, 32}, Vector2Int{300, 20} }));
    pw = pwInputptr.get();
    uiManager.AddUI(std::move(pwInputptr));

    std::unique_ptr<Button> logInBtnptr = std::make_unique<Button>(L"", Transform2DINT({ Position{1250, 10}, Vector2Int{70, 44} }));
    logInBtn = logInBtnptr.get();
    uiManager.AddUI(std::move(logInBtnptr));

    std::unique_ptr<Button> logOutBtnptr = std::make_unique<Button>(L"", Transform2DINT({ Position{1330, 10}, Vector2Int{70, 44} }));
    logOutBtn = logOutBtnptr.get();
    uiManager.AddUI(std::move(logOutBtnptr));


    // 입력창
    std::unique_ptr<WindowUI> result = std::make_unique<WindowUI>(L"", Transform2DINT({ Position{940, 416}, Vector2Int{460, 250} }));
    resultLog = result.get();
    uiManager.AddUI(std::move(result));

    std::unique_ptr<WindowUI> usedTimeptr = std::make_unique<WindowUI>(L"", Transform2DINT({ Position{940, 396}, Vector2Int{200, 20} }));
    usedTime = usedTimeptr.get();
    uiManager.AddUI(std::move(usedTimeptr));

    std::unique_ptr<WindowUI> numOfColumnptr = std::make_unique<WindowUI>(L"", Transform2DINT({ Position{1200, 396}, Vector2Int{160, 20} }));
    numOfColumns = numOfColumnptr.get();
    uiManager.AddUI(std::move(numOfColumnptr));

    std::unique_ptr<Button> resultClear = std::make_unique<Button>(L"", Transform2DINT({ Position{1320, 676}, Vector2Int{80, 30} }));
    resultClearButton = resultClear.get();
    uiManager.AddUI(std::move(resultClear));



    std::unique_ptr<WindowUI> edit = std::make_unique<WindowUI>(L"", Transform2DINT({ Position{940, 65}, Vector2Int{460, 250} }));
    editUI = edit.get();
    uiManager.AddUI(std::move(edit));

    std::unique_ptr<Button> submit = std::make_unique<Button>(L"", Transform2DINT({ Position{1240, 329}, Vector2Int{160, 30} })); 
    submitButton = submit.get();
    uiManager.AddUI(std::move(submit));

    std::unique_ptr<Toggle> autoCommitTgl = std::make_unique<Toggle>(L"", Transform2DINT({ Position{940, 329}, Vector2Int{80, 30} }));
    autoCommitToggle = autoCommitTgl.get();
    uiManager.AddUI(std::move(autoCommitTgl));

    std::unique_ptr<Button> rollback = std::make_unique<Button>(L"", Transform2DINT({ Position{1030, 329}, Vector2Int{80, 30} }));
    rollbackButton = rollback.get();
    uiManager.AddUI(std::move(rollback));

    std::unique_ptr<Button> commit = std::make_unique<Button>(L"", Transform2DINT({ Position{1120, 329}, Vector2Int{80, 30} }));
    commitButton = commit.get();
    uiManager.AddUI(std::move(commit));


    // 트리 뷰
    std::unique_ptr<TreeView> tableTreeViewUI = std::make_unique<TreeView>(L"", Transform2DINT({ Position{640, 65}, Vector2Int{290, 600} }));
    hTreeView = tableTreeViewUI.get();
    uiManager.AddUI(std::move(tableTreeViewUI));

    std::unique_ptr<Button> refresh = std::make_unique<Button>(L"", Transform2DINT({ Position{640, 674}, Vector2Int{100, 30} }));
    refreshButton = refresh.get();
    uiManager.AddUI(std::move(refresh));


      
    std::unique_ptr<HistoryListBox> prevLogBox = std::make_unique<HistoryListBox>(L"", Transform2DINT({ Position{0, 0}, Vector2Int{0, 0} }));
    prevQueryListBox = prevLogBox.get();
    uiManager.AddUI(std::move(prevLogBox));
    

    // 테이블
    std::unique_ptr<TableUI> listViewUI = std::make_unique<TableUI>(L"", Transform2DINT({ Position{10, 10}, Vector2Int{620, 710} }));
    listView = listViewUI.get();
    uiManager.AddUI(std::move(listViewUI));



    //// 폰트 설정
    ZeroMemory(&g_defaultCF, sizeof(g_defaultCF));
    g_defaultCF.cbSize = sizeof(g_defaultCF);

    // 활성화할 항목들 마스크 (폰트명, 크기, 색상, 굵기, 문자셋)
    g_defaultCF.dwMask = CFM_BOLD | CFM_FACE | CFM_SIZE | CFM_COLOR | CFM_WEIGHT | CFM_CHARSET;

    // 1. 폰트명
    wcscpy_s(g_defaultCF.szFaceName, L"맑은 고딕");

    // 2. 크기 (원하는 포인트 단위 * 20)
    // 예: 11pt를 원하면 11 * 20 = 220
    // 만약 픽셀(18px) 기준이라면 MulDiv(18, 1440, GetDeviceCaps(GetDC(hRichEdit), LOGPIXELSY)) 사용
    g_defaultCF.yHeight = 220;

    // 3. 기타 속성
    g_defaultCF.crTextColor = RGB(0, 0, 0); // 검정
    g_defaultCF.wWeight = FW_NORMAL;
    g_defaultCF.bCharSet = DEFAULT_CHARSET;

    WindowEX::InitializeWindow(title, wndProc);
    return true;
}
void DatabaseWindow::WM_CREATE_FUNC(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // 메뉴 생성
    hMenuBar = CreateMenu();

    // 파일 서브 메뉴 생성
    //HMENU hFileMenu = CreatePopupMenu();
    //AppendMenuW(hFileMenu, MF_STRING, ID_MENU_LOAD_QUERY, L"쿼리 불러오기(&O)");
    //AppendMenuW(hFileMenu, MF_STRING, ID_MENU_SAVE_QUERY, L"쿼리 저장하기(&S)");
    //AppendMenuW(hFileMenu, MF_SEPARATOR, 0, NULL); // 구분선
    //AppendMenuW(hFileMenu, MF_STRING, ID_MENU_EXIT, L"종료(&X)");
    //AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hFileMenu, L"파일(&F)"); // 마지막에 서브 메뉴 붙이기

    // 히스토리 서브 메뉴 생성
    HMENU hHistoryMenu = CreatePopupMenu();
    AppendMenuW(hHistoryMenu, MF_STRING, ID_MENU_CLEAR_HISTORY, L"기록 삭제");
    AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hHistoryMenu, L"히스토리(&H)"); // 마지막에 서브 메뉴 붙이기

    // 플래그 메뉴 생성
    HMENU hFlagMenu = CreatePopupMenu();
    AppendMenuW(hHistoryMenu, MF_STRING, ID_MENU_CLEAR_HISTORY, L"기록 삭제");
    AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hHistoryMenu, L"히스토리(&H)"); // 마지막에 서브 메뉴 붙이기

    // 윈도우에 메뉴바 적용
    SetMenu(hwnd, hMenuBar);
    
    // id 창
    id->Create(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, hwnd, nullptr, WindowEX::hInstance);
    id->SetFont(hFontNormal);

    pw->Create(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_PASSWORD, hwnd, nullptr, WindowEX::hInstance);
    pw->SetFont(hFontNormal);

    logInBtn->Create(0, L"BUTTON", L"Log In", WS_CHILD | WS_VISIBLE |  BS_PUSHBUTTON, hwnd, (HMENU)ID_LOGIN, hInstance);
    logInBtn->SetFont(hFontBold);

    logOutBtn->Create(0, L"BUTTON", L"Log Out", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, hwnd, (HMENU)ID_LOGOUT, hInstance);
    logOutBtn->SetFont(hFontBold);

    currIDTag->Create(0, L"STATIC", L"User ID:", WS_CHILD | WS_VISIBLE, hwnd, nullptr, WindowEX::hInstance);
    currIDTag->SetFont(hFontBold);

    currDatabaseTag->Create(0, L"STATIC", L"Database:", WS_CHILD | WS_VISIBLE, hwnd, nullptr, WindowEX::hInstance);
    currDatabaseTag->SetFont(hFontBold);

    currID->Create(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE, hwnd, nullptr, WindowEX::hInstance);
    currID->SetFont(hFontBold);
  
    currDatabase->Create(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE, hwnd, nullptr, WindowEX::hInstance);
    currDatabase->SetFont(hFontBold);


    // query 입력 창
    LoadLibrary(L"Msftedit.dll"); // Rich Edit 4.1 버전 사용

    editUI->Create(WS_EX_CLIENTEDGE, MSFTEDIT_CLASS, L"", WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | WS_VSCROLL | ES_MULTILINE | ES_WANTRETURN, hwnd, (HMENU)ID_EDIT, WindowEX::hInstance);
    editUI->SendToHWND(EM_SETEVENTMASK, 0, ENM_CHANGE);
    editUI->SendToHWND(EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&g_defaultCF);
    SetWindowSubclass(editUI->GetHWND(), RichEditSubProc, 1, (DWORD_PTR)nullptr);

    submitButton->Create(0, L"BUTTON", L"Submit", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP, hwnd, (HMENU)ID_SUBMIT, hInstance);
    submitButton->SetFont(hFontBold);

    refreshButton->Create(0, L"BUTTON", L"Refresh", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, hwnd, (HMENU)ID_REFRESH, hInstance);
    refreshButton->SetFont(hFontBold);

    autoCommitToggle->Create(0, L"BUTTON", L"Transaction", WS_VISIBLE | WS_CHILD | BS_PUSHLIKE | BS_AUTOCHECKBOX | BS_OWNERDRAW, hwnd, (HMENU)ID_AUTOCOMMIT, hInstance);
    autoCommitToggle->SetToggled(false);
    autoCommitToggle->SetFont(hFontBold);

    commitButton->Create(0, L"BUTTON", L"Commit", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, hwnd, (HMENU)ID_COMMIT, hInstance);
    commitButton->SetFont(hFontBold);

    rollbackButton->Create(0, L"BUTTON", L"Rollback", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW, hwnd, (HMENU)ID_ROLLBACK, hInstance);
    rollbackButton->SetFont(hFontBold);

    // 결과 및 에러 출력 창
    resultLog->Create(WS_EX_CLIENTEDGE, MSFTEDIT_CLASS, L"", WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | WS_VSCROLL | ES_MULTILINE | ES_READONLY, hwnd, (HMENU)ID_RESULT_LOG, WindowEX::hInstance);
    resultLog->SetFont(hFontNormal);
    resultLog->SendToHWND(EM_SETBKGNDCOLOR, 0, (LPARAM)RGB(230, 230, 230));

   
    usedTime->Create(0, L"STATIC", L"Time spent (ms): 0.0000", WS_CHILD | WS_VISIBLE, hwnd, (HMENU)ID_USED_TIME, WindowEX::hInstance);
    usedTime->SetFont(hFontBold);
    numOfColumns->Create(0, L"STATIC", L"Rows: 0", WS_CHILD | WS_VISIBLE, hwnd, (HMENU)ID_NUMOF_COLUMNS, WindowEX::hInstance);
    numOfColumns->SetFont(hFontBold);

    resultClearButton->Create(0, L"BUTTON", L"Clear", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, hwnd, (HMENU)ID_CLEAR_RESULT, hInstance);
    resultClearButton->SetFont(hFontBold);

    // table
    listView->Create(WS_EX_CLIENTEDGE, WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | ES_READONLY | LVS_REPORT | LVS_SINGLESEL | LVS_OWNERDATA, hwnd, (HMENU)ID_LIST_VIEW, hInstance);
    hTreeView->Create(0, WC_TREEVIEW, L"Tree View", WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT, hwnd, (HMENU)5000, hInstance);  

    //prevQueryListBox->Create(0, WC_LISTBOX, L"prevList", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_HSCROLL | LBS_NOTIFY, hwnd, (HMENU)ID_PREV_QUERYLIST, hInstance);
    //prevQueryListBox->SetFont(hFontBold);
}

//////////////////////////////////////////////////////////// account ////////////////////////////////////////////////////////////
void DatabaseWindow::LogIn(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    try
    {
        if (account->IsConnected()) throw L"Already Logged-in.";

        std::wstring idText = id->GetTextWFromHWND();
        std::wstring pwText = pw->GetTextWFromHWND();

        bool result = account->Connect("localhost", WStringToUTF8(idText).c_str(), WStringToUTF8(pwText), "None");
        if (!result)
        {
            const std::wstring errMsg = account->GetLastErrorW();
            account->Close();
            throw errMsg;
        }

        currID->SendToHWND(WM_SETTEXT, 0, (LPARAM)idText.c_str());
        currDatabase->SendToHWND(WM_SETTEXT, 0, (LPARAM)UTF8ToWString("None").c_str());
        WriteMsg(L"Log In success", false);
        RefreshTree();
        
    }
    catch (const std::wstring& msg)
    {
        WriteMsg(msg, true);
    }
    id->SendToHWND(WM_SETTEXT, 0, (LPARAM)L""); // 문자 초기화
    pw->SendToHWND(WM_SETTEXT, 0, (LPARAM)L""); // 문자 초기화
}
void DatabaseWindow::LogOut(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (!account->IsConnected())
    {
        WriteMsg(L"No connection", true);
        return;
    }
    account->Close();
    currID->SendToHWND(WM_SETTEXT, 0, (LPARAM)L"");
    currDatabase->SendToHWND(WM_SETTEXT, 0, (LPARAM)L"None");
    listView->Clear();
    hTreeView->DeleteAll();
    WriteMsg(L"Log Out", false);
}

//////////////////////////////////////////////////////////// query process ////////////////////////////////////////////////////////////
my_ulonglong DatabaseWindow::WorkQueryProcess(const std::wstring& query)
{
    timer.Start();
    my_ulonglong queryResult = account->ExecuteQuery(query);
    if (queryResult == -1) return queryResult;

    listView->Clear(); // 성공 시에만 기존 테이블 제거

    MYSQL_FIELD* fields = account->getFieldName();
    unsigned int num_fields = account->getFieldNum();

    std::vector<ColumnData> columns;
    for (unsigned int i = 0; i < num_fields; ++i)
    {
        ColumnType type = ColumnType::Normal;
        if (fields[i].flags & PRI_KEY_FLAG) type = ColumnType::PK;
        else if (fields[i].flags & MULTIPLE_KEY_FLAG) type = ColumnType::FK;
        columns.push_back({ type, UTF8ToWString(fields[i].name) });
    }
       
    listView->SetColumns(columns);

    MYSQL_ROW row = nullptr;
    int count = 0;
    while ((row = account->fetchRow()) && count < account->GetMaxRow())
    {
        std::vector<CellData> oneRow;

        // 각 필드는 문자열(char*) 형태로 반환됨
        for (unsigned int i = 0; i < num_fields; ++i)  // 각 row의 field를 출력
        {
            bool isRealNULL = row[i] ? false : true;
            std::wstring value = isRealNULL ? L"NULL" : UTF8ToWString(row[i]);
            
            CellData cell{ fields[i].type, value, isRealNULL };
            oneRow.push_back(cell);
        }
        listView->AddRow(oneRow);
        count++;
    }

    listView->SetItemCount();
    listView->Resize();
    
    timer.End();
    double ms = timer.GetDuration(); 
    UpdateUsedTimeAndColumns(ms, queryResult);
    WriteQueryResult(query, ms);
    return queryResult;
}
void DatabaseWindow::SendQuery(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    std::wstring query = editUI->GetTextWFromHWND();
    my_ulonglong result = 0;
    try
    {
        result = WorkQueryProcess(query);

        if (result == -1) throw false;

        currDatabase->SendToHWND(WM_SETTEXT, 0, (LPARAM)account->GetDatabaseName().c_str()); // 현재 데이터베이스 이름 출력
        prevQueryListBox->AddQuery(query); // 사용 쿼리 저장
        prevQueryListBox->SaveCurrQuery(L"");
        editUI->SendToHWND(WM_SETTEXT, 0, (LPARAM)L""); // 문자 초기화
    }
    catch (...)
    {
        WriteMsg(account->GetLastErrorW(), true);
    }
   
}

//////////////////////////////////////////////////////////// table & tree ////////////////////////////////////////////////////////////
bool DatabaseWindow::RefreshTree()
{
    hTreeView->DeleteAll(); //  먼저 삭제
    if (account->ExecuteQuery(L"SHOW DATABASES") == -1)
    {
        WriteMsg(account->GetLastErrorW(), true);
        return false;
    }
    MYSQL_RES* dbRes = account->GetResult();
    MYSQL_ROW row;


    // 우선 DB 이름 다 가져오기
    std::vector<std::wstring> dbNames;
    while ((row = account->fetchRowFromOutside(dbRes)))
    {
        if (row[0])
        {
            dbNames.push_back(UTF8ToWString(row[0]));
        }
    }

    for (const auto& name : dbNames)
    {
        HTREEITEM hDbItem = hTreeView->AddItem(TVI_ROOT, name);

        std::string query = "SHOW TABLES FROM " + WStringToUTF8(name);

        if (account->ExecuteQuery(query.c_str()) == -1) continue;

        MYSQL_RES* tableRes = account->GetResult();
        MYSQL_ROW tableRow;
        while ((tableRow = account->fetchRowFromOutside(tableRes)))
        {
            if (tableRow[0])
            {
                hTreeView->AddItem(hDbItem, UTF8ToWString(tableRow[0]));
            }
        }
    }

    // 화면 갱신 후 다시 그리기. 깜박임 방지
    hTreeView->SendToHWND(WM_SETREDRAW, TRUE, 0); //SendMessage(hTreeView->GetHWND(), WM_SETREDRAW, TRUE, 0);
    hTreeView->Invalidate();
    return true;
}
void DatabaseWindow::NotifyTreeClick(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    
    LPNMHDR lpnmhdr = (LPNMHDR)lParam;
    // 트리뷰에서 보낸 메시지인지 확인
    if (lpnmhdr->hwndFrom == hTreeView->GetHWND())
    {
        switch (lpnmhdr->code)
        {
        case NM_DBLCLK: // 항목 선택이 변경되었을 때
        {
            // 1. 현재 선택된 아이템(테이블) 핸들 가져오기
            HTREEITEM hSelected = TreeView_GetSelection(hTreeView->GetHWND());
            if (hSelected == NULL) break;

            if (hSelected != NULL)
            {
          
                // 자식 이름
                wchar_t itemText[256] = { 0 };
                TVITEM tvi = { 0 };
                tvi.hItem = hSelected;
                tvi.mask = TVIF_TEXT; 
                tvi.pszText = itemText;
                tvi.cchTextMax = 256;

                if (TreeView_GetItem(hTreeView->GetHWND(), &tvi))
                {
                    // 부모 이름
                    HTREEITEM hParent = TreeView_GetParent(hTreeView->GetHWND(), hSelected);
                    wchar_t dbName[256] = { 0 };
                    TVITEM tviParent = { 0 };
                    tviParent.hItem = hParent;
                    tviParent.mask = TVIF_TEXT;
                    tviParent.pszText = dbName;
                    tviParent.cchTextMax = 256;
                    TreeView_GetItem(hTreeView->GetHWND(), &tviParent);

                    if (hParent != NULL)
                    {
                        std::wstring db = dbName;
    
                        if (db != currDatabase->GetTextWFromHWND())
                        {
                            if (WorkQueryProcess(L"USE " + db + L";") == -1) WriteMsg(account->GetLastErrorW(), true);
                            else currDatabase->SendToHWND(WM_SETTEXT, 0, (LPARAM)account->GetDatabaseName().c_str());
                        }
              

                        std::wstring tableName = itemText;
                        if (WorkQueryProcess(L"SELECT * FROM " + tableName + L";") == -1) WriteMsg(account->GetLastErrorW(), true);
                        
                    }
                }
            }
        }
        break;
        }
    }
}
void DatabaseWindow::NotifyTableMaking(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LPNMHDR pnmh = (LPNMHDR)lParam;
    try
    {
        if (pnmh->code == LVN_GETDISPINFO) {
            NMLVDISPINFO* plvdi = (NMLVDISPINFO*)lParam;

            if (plvdi->item.mask & LVIF_TEXT) {
                int r = plvdi->item.iItem;      // 행 번호
                int c = plvdi->item.iSubItem;   // 열 번호

                // 안전장치: 벡터 범위를 벗어나지 않는지 확인
               // 1. 행 인덱스 검사
                if (r >= listView->GetRowSize()) throw false;

                // 2. 열 인덱스 검사
                if (c >= listView->GetColumnSize()) {
                    plvdi->item.pszText = (LPWSTR)L"NULL"; // 데이터가 없으면 null
                    throw false;
                }

                // 3. 실제 데이터 연결
                plvdi->item.pszText = (LPWSTR)listView->GetRealItem(r, c).value.c_str();
            }
        }
    }
    catch (...)
    {

    }
    //timer.GetDuration();
}
uint32_t DatabaseWindow::NotifyTableColoring(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LPNMHDR pnmh = (LPNMHDR)lParam;
    
    if (pnmh->idFrom == ID_LIST_VIEW && pnmh->code == NM_CUSTOMDRAW)
    {
        LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)lParam;
        switch (lplvcd->nmcd.dwDrawStage)
        {
        case CDDS_PREPAINT:
            return CDRF_NOTIFYITEMDRAW; // 아이템 단위로 통보 받음

        case CDDS_ITEMPREPAINT:
            return CDRF_NOTIFYSUBITEMDRAW; // 서브 아이템(COLUMN) 단위로 통보 받음


        case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
        {
            int row = (int)lplvcd->nmcd.dwItemSpec;
            int col = lplvcd->iSubItem;


            /// column 서식
            ColumnData colData = listView->GetColumnData(col);
            if (colData.type == ColumnType::PK)
            {
 
                lplvcd->clrTextBk = RGB(200, 255, 200); // 배경: 연한 초록 (FK 느낌)
            }
            else if (colData.type == ColumnType::FK)
            {
                lplvcd->clrTextBk = RGB(200, 255, 255); // 배경: 연한 초록 (FK 느낌)
            }
            else
            {       
                lplvcd->clrTextBk = RGB(255, 255, 255);
            }

            /// 실제 데이터 서식
            // 데이터 벡터에서 셀의 텍스트 가져옴
            CellData value = listView->GetRealItem(row, col);
            if (value.isRealNULL)
            {
                SelectObject(lplvcd->nmcd.hdc, hFontBoldItalic); // 이탤릭으로 바꿈
                lplvcd->clrText = RGB(255, 0, 0);
            }

            else if (value.IsNum())
            {
                SelectObject(lplvcd->nmcd.hdc, hFontNormal);
                lplvcd->clrText = RGB(0, 0, 180);
            }

            else if (value.IsTime())
            {
                SelectObject(lplvcd->nmcd.hdc, hFontNormal);
                lplvcd->clrText = RGB(0, 144, 0);
                //SelectObject(lplvcd->nmcd.hdc, hFontBold); 
            }

            else // 문자열
            {
                SelectObject(lplvcd->nmcd.hdc, hFontNormal);
                lplvcd->clrText = RGB(0, 0, 0);
            }
            return CDRF_DODEFAULT;
        }
        }


    }

    return CDRF_DODEFAULT;
}

//////////////////////////////////////////////////////////// transaction ////////////////////////////////////////////////////////////
void DatabaseWindow::SetTransactionMode(const TransactionType& type)
{
    bool result = true;
    std::wstring resultLog;
    switch (type)
    {
    case TransactionType::Start:
        result = account->StartTransaction();
        autoCommitToggle->SetToggled(true);
        resultLog = result == true ? L"Start Transaction" : account->GetLastErrorW();
        break;

    case TransactionType::Commit:
        result = account->Commit();
        autoCommitToggle->SetToggled(false);
        resultLog = result == true ? L"Commit" : L"Commit Error " + account->GetLastErrorW();
        break;

    case TransactionType::Rollback:
        result = account->Rollback();
        autoCommitToggle->SetToggled(false);
        resultLog = result == true ? L"Rollback" : L"Rollback Error " + account->GetLastErrorW();
        break;
    }

    WriteMsg(resultLog, !result);
}
void DatabaseWindow::SetTransactionMode(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (!account->IsConnected())
    {
        WriteMsg(L"No connection", true);
        return;
    }

    std::wstring resultLog;
    if (!autoCommitToggle->IsToggled())
    {
        resultLog = L"Start Transaction";
        account->StartTransaction(); // 클래스 메서드 호출
        autoCommitToggle->SetToggled(true);
    }
    else
    {
        resultLog = L"Stop Transaction";
        if (account->IsDirty())
        {
            int result = MessageBox(hwnd,
                L"커밋되지 않은 데이터가 있습니다. 저장하시겠습니까?",
                L"트랜잭션 종료 확인",
                MB_YESNO | MB_ICONQUESTION);

            if (result == IDYES)
            {
                account->Commit();
                autoCommitToggle->SetToggled(false);
            }
            else 
            {
                autoCommitToggle->SendToHWND(BM_SETCHECK, BST_CHECKED, 0);
                return;
            }
        }
        else
        {
            autoCommitToggle->SetToggled(false);
        }
    }

    WriteMsg(resultLog);
}
void DatabaseWindow::DrawAutoTransaction(LPDRAWITEMSTRUCT lpDrawItem, HDC hdc, RECT rect)
{
    if (lpDrawItem->CtlID == ID_AUTOCOMMIT) // 커밋 버튼이라면
    {
        bool isToggled = autoCommitToggle->IsToggled();
        COLORREF bgdColor = isToggled ? RGB(160, 0, 0) : GetSysColor(COLOR_BTNFACE);
        COLORREF txtColor = isToggled ? RGB(255, 255, 255) : RGB(0, 0, 0);

        // 배경색 칠하기
        HBRUSH hBrush = CreateSolidBrush(bgdColor);
        FillRect(hdc, &rect, hBrush);
        DeleteObject(hBrush);

        // 눌렸을 때 테두리 표현 등
        if (lpDrawItem->itemState & ODS_SELECTED)
        {
            DrawEdge(hdc, &rect, EDGE_SUNKEN, BF_RECT);
        }
        else
        {
            DrawEdge(hdc, &rect, EDGE_RAISED, BF_RECT);
        }

        HFONT hFont = (HFONT)SendMessage(lpDrawItem->hwndItem, WM_GETFONT, 0, 0);
        if (!hFont) hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

        // 글자 색 설정
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, txtColor);

        // 버튼 텍스트 그리기
        wchar_t buf[256];
        GetWindowText(lpDrawItem->hwndItem, buf, 256);
        DrawText(hdc, buf, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        int showCmd = isToggled ? SW_SHOW : SW_HIDE;
        // hBtnCommit, hBtnRollback은 버튼 생성 시 저장해둔 HWND입니다.
        ShowWindow(commitButton->GetHWND(), showCmd);
        ShowWindow(rollbackButton->GetHWND(), showCmd);
    }

    if (lpDrawItem->CtlID == ID_ROLLBACK)
    {

        // 배경색 칠하기
        HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
        FillRect(hdc, &rect, hBrush);
        DeleteObject(hBrush);


        // 눌렸을 때 테두리 표현 등
        if (lpDrawItem->itemState & ODS_SELECTED)
        {
            DrawEdge(hdc, &rect, EDGE_SUNKEN, BF_RECT);
        }
        else
        {
            DrawEdge(hdc, &rect, EDGE_RAISED, BF_RECT);
        }


        HFONT hFont = (HFONT)SendMessage(lpDrawItem->hwndItem, WM_GETFONT, 0, 0);
        if (!hFont) hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

        // 글자 색 설정
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 0, 0));

        // 버튼 텍스트 그리기
        wchar_t buf[256];
        GetWindowText(lpDrawItem->hwndItem, buf, 256);
        DrawText(hdc, buf, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

//////////////////////////////////////////////////////////// Edit Box ////////////////////////////////////////////////////////////
LRESULT CALLBACK DatabaseWindow::RichEditSubProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    switch (msg)
    {
    case WM_KEYDOWN:
    {
        bool isCtrl = (GetKeyState(VK_CONTROL) & 0x8000);
        bool hasQuery = prevQueryListBox->GetCount() != 0;
        if (isCtrl && hasQuery)
        {
            if (prevQueryListBox->GetCurrIndex() == -1)
            {
                std::wstring query = editUI->GetTextWFromHWND();
                prevQueryListBox->SaveCurrQuery(query);
            }

            if (wParam == VK_UP) 
            {
                prevQueryListBox->TravelIndex(1); 
                editUI->SendToHWND(WM_SETTEXT, 0, (LPARAM)prevQueryListBox->GetTextFromIndex().c_str());
                return 0; // RichEdit의 기본 동작(커서 이동) 무시
            }
            else if (wParam == VK_DOWN)
            {
                prevQueryListBox->TravelIndex(-1);
                editUI->SendToHWND(WM_SETTEXT, 0, (LPARAM)prevQueryListBox->GetTextFromIndex().c_str());
                return 0; // RichEdit의 기본 동작(커서 이동) 무시
            }

           

            //ShowResultMsg(L"Curr Index: " + std::to_wstring(prevQueryListBox->GetCurrIndex()));
         
        }
        break;
    }

    case WM_PASTE:
    {
        if (OpenClipboard(hwnd))
        { // hwnd는 매개변수로 들어온 RichEdit 핸들
            HANDLE hData = GetClipboardData(CF_UNICODETEXT);
            if (hData)
            {
                wchar_t* pText = static_cast<wchar_t*>(GlobalLock(hData));
                if (pText)
                {
                    SendMessage(hwnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&g_defaultCF);
                    SendMessage(hwnd, EM_REPLACESEL, TRUE, (LPARAM)pText);
                    //SendMessage(hwnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&g_defaultCF);
                    GlobalUnlock(hData);
                }
            }
            CloseClipboard();
            return 0; // RichEdit의 자체적인 서식 포함 붙여넣기 방지
        }
        break;
    }
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}
void DatabaseWindow::ApplySqlHighlight()
{
    const HWND& hRichEdit = editUI->GetHWND();
    SendMessage(hRichEdit, WM_SETREDRAW, FALSE, 0);

    // 현 커서 위치 및 스크롤 위치 저장
    CHARRANGE crOriginal;
    POINT scrollPos;
    SendMessage(hRichEdit, EM_EXGETSEL, 0, (LPARAM)&crOriginal);
    SendMessage(hRichEdit, EM_GETSCROLLPOS, 0, (LPARAM)&scrollPos);


    // RichEdit에서 텍스트 가져오기 (CRLF를 LF로 변환하여 가져옴, 색 밀림 방지용)
    GETTEXTEX gt = { 0 };
    gt.cb = (GetWindowTextLength(hRichEdit) + 1) * sizeof(wchar_t);
    gt.flags = GT_DEFAULT;
    gt.codepage = 1200;    // Unicode (UTF-16)

    std::wstring text;
    text.resize(GetWindowTextLength(hRichEdit));
    SendMessage(hRichEdit, EM_GETTEXTEX, (LPARAM)&gt, (LPARAM)&text[0]);
    size_t len = text.length();

    // 검은색으로 먼저 초기화
    SendMessage(hRichEdit, EM_SETSEL, 0, -1);
    SendMessage(hRichEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&g_defaultCF);

    // 각 키워드 탐색 및 색상 적용
    CHARFORMAT2 cfKey = g_defaultCF;

    // 대문자로 변환된 비교용 문자열 만들기
    std::wstring upperText = text;
    std::transform(upperText.begin(), upperText.end(), upperText.begin(), towupper);

    for (const auto& group : queryExample.keywordGroups)
    {
        cfKey.crTextColor = group.color;
        if (group.isBold) cfKey.dwEffects |= CFE_BOLD;
        else cfKey.dwEffects &= ~CFE_BOLD;

        for (const auto& word : group.words)
        {
            size_t pos = 0;

            while ((pos = upperText.find(word, pos)) != std::wstring::npos)
            {
                bool isStandalone = true;
                if (pos > 0 && iswalnum(text[pos - 1])) isStandalone = false;
                if (pos + word.length() < len && iswalnum(text[pos + word.length()])) isStandalone = false;

                if (isStandalone)
                {
                    SendMessage(hRichEdit, EM_SETSEL, pos, pos + word.length());
                    SendMessage(hRichEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cfKey);
                }
                pos += word.length();
            }
        }
    }
   
    // 커서 및 스크롤 위치 복구
    SendMessage(hRichEdit, EM_EXSETSEL, 0, (LPARAM)&crOriginal);
    SendMessage(hRichEdit, EM_SETSCROLLPOS, 0, (LPARAM)&scrollPos);

    // 화면 업데이트 재개 및 즉시 갱신
    SendMessage(hRichEdit, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(hRichEdit, NULL, TRUE);
}

//////////////////////////////////////////////////////////// Log ////////////////////////////////////////////////////////////
void DatabaseWindow::WriteMsg(const std::wstring str, bool isError)
{
    std::wstring time = GetTimeString();
    //std::wstring fixed = fixedColumns == -1 || fixedColumns == 0 ? L"" : L" [" + std::to_wstring(fixedColumns) + L" columns]";
    std::wstring final = L" " + str + L"\r\n";

    int length = GetWindowTextLengthW(resultLog->GetHWND());
    resultLog->SendToHWND(EM_SETSEL, length, length); // 끝으로 이동

    // 2. 서식 설정 (CHARFORMAT2)
    CHARFORMAT2 cf;
    ZeroMemory(&cf, sizeof(cf));
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_COLOR | CFM_BOLD | CFM_FACE;
    cf.dwEffects = CFE_BOLD;         // 볼드체 효과 ON
    cf.crTextColor = RGB(120, 120, 120);

    // 시간 넣기
    resultLog->SendToHWND(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf); // 해당 줄의 텍스트 색 교체
    resultLog->SendToHWND(EM_REPLACESEL, FALSE, (LPARAM)time.c_str()); // 텍스트 넣기

    // 볼드 해제 및 실 내용 넣기
    cf.dwEffects = 0;                // 볼드체 효과 OFF
    cf.crTextColor = isError ? RGB(255, 0, 0) : RGB(0, 0, 0);        // 성공/실패 색상 적용
    resultLog->SendToHWND(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf); // 해당 줄의 텍스트 색 교체
    resultLog->SendToHWND(EM_REPLACESEL, FALSE, (LPARAM)final.c_str()); // 텍스트 넣기

    resultLog->SendToHWND(EM_SETSEL, -1, -1);
    resultLog->SendToHWND(WM_VSCROLL, SB_BOTTOM, 0);
}
void DatabaseWindow::WriteQueryResult(const std::wstring query, double ms)
{
    wchar_t buffer[32] = { 0 };
    swprintf_s(buffer, L" [%.4f ms]", ms);
    std::wstring msStr = std::to_wstring(ms);
    std::wstring resultLog = query; resultLog += buffer;
    WriteMsg(resultLog);
}
void DatabaseWindow::UpdateUsedTimeAndColumns(double ms, my_ulonglong columns)
{
    wchar_t buffer[32] = { 0 };
    swprintf_s(buffer, L"%.4f", ms);
    std::wstring timeResult = L"Time spent (ms): "; timeResult += buffer;
    std::wstring columnResult = L"Rows: " + std::to_wstring(columns);

    usedTime->SendToHWND(WM_SETTEXT, 0, (LPARAM)timeResult.c_str());
    numOfColumns->SendToHWND(WM_SETTEXT, 0, (LPARAM)columnResult.c_str());
}

//////////////////////////////////////////////////////////// Others ////////////////////////////////////////////////////////////
void DatabaseWindow::RefreshAll()
{
    RefreshTree();
}




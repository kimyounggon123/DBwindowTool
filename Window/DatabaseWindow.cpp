#include "DatabaseWindow.h"

// 여러 ui들
WindowUI* editUI = nullptr;
WindowUI* resultWnd = nullptr;
WindowUI* submitButton = nullptr;



TableUI* listView = nullptr;


void TableUI::SetColumns(const std::vector<std::wstring>& cols)
{
    LVCOLUMN col{};
    col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    columnSize = cols.size();
    for (int i = 0; i < columnSize; ++i)
    {
        col.pszText = (LPWSTR)cols[i].c_str();
        col.cx = 120;
        col.iSubItem = i;

        SendMessageW(winUI, LVM_INSERTCOLUMN, i, (LPARAM)&col);
        //ListView_SetColumnWidth(winUI, i, LVSCW_AUTOSIZE_USEHEADER);
    }
    
}
void TableUI::AddRow(const std::vector<std::wstring>& data)
{
    if (data.size() != columnSize) return;
    int index = SendMessageW(winUI, LVM_GETITEMCOUNT, 0, 0); // 현재 몇 개의 행이 있는가?

    LVITEM item{};
    item.mask = LVIF_TEXT;
    item.iItem = index;
    item.pszText = (LPWSTR)data[0].c_str();

    rowSize = index;

    SendMessageW(winUI, LVM_INSERTITEM, 0, (LPARAM)&item);
    for (int i = 1; i < data.size(); ++i)
    {
        LVITEM sub{};
        sub.iSubItem = i;
        sub.pszText = (LPWSTR)data[i].c_str();

        SendMessageW(winUI, LVM_SETITEMTEXT, index, (LPARAM)&sub);
    }
}

void TableUI::Clear()
{
    SendMessageW(winUI, LVM_DELETEALLITEMS, 0, 0);
}

bool DatabaseWindow::InitializeWindow(const wchar_t* title, WNDPROC wndProc)
{

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
    std::unique_ptr<WindowUI> edit = std::make_unique<WindowUI>
        (L"", Transform2DINT({ Position{200, 400}, Vector2Int{500, 100} , 0.0f }));
    editUI = edit.get();
    uiManager.AddUI(std::move(edit));

    std::unique_ptr<WindowUI> result = std::make_unique<WindowUI>
        (L"", Transform2DINT({ Position{200, 200}, Vector2Int{500, 100} , 0.0f }));
    resultWnd = result.get();
    uiManager.AddUI(std::move(result));

    std::unique_ptr<WindowUI> submit = std::make_unique<WindowUI>
        (L"", Transform2DINT({ Position{300, 300}, Vector2Int{100, 20} , 0.0f }));
    submitButton = submit.get();
    uiManager.AddUI(std::move(submit));

    std::unique_ptr<TableUI> listViewUI = std::make_unique<TableUI>
        (L"", Transform2DINT({ Position{10, 10}, Vector2Int{600, 600} , 0.0f }));
    listView = listViewUI.get();
    uiManager.AddUI(std::move(listViewUI));


}

void DatabaseWindow::Shutdown()
{
}

LRESULT CALLBACK DatabaseWindow::DBMain(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UImanager& ui = UImanager::GetInstance();

    switch (msg)
    {
    case WM_CREATE:
    {
        WM_CREATE_FUNC(hwnd, msg, wParam, lParam);
        break;
    }

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
        {
            wchar_t buffer[4096];
            GetWindowText(editUI->GetHWND(), buffer, 4096);
            std::wstring query = buffer;
            int len = GetWindowTextLength(editUI->GetHWND());
            resultWnd->SendToHWND(EM_REPLACESEL, FALSE, (LPARAM)query.c_str());
            editUI->SendToHWND(EM_SETSEL, len, len);
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

void DatabaseWindow::WM_CREATE_FUNC(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    /*
        editUI->Create(
            WS_EX_CLIENTEDGE,
            L"EDIT",
            L"",
            WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
            hwnd,
            (HMENU)1001,
            WindowEX::hInstance
        );
        editUI->SetFont(hFontNormal);

        resultWnd->Create(
            WS_EX_CLIENTEDGE,
            L"EDIT",
            L"",
            WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL | ES_READONLY,
            hwnd,
            (HMENU)1002,
            WindowEX::hInstance
        );
        resultWnd->SetFont(hFontNormal);

        submitButton->Create(
            0,
            L"BUTTON",
            L"Submit",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            hwnd,             // 부모
            (HMENU)1003,      // ID
            hInstance
            );
        submitButton->SetFont(hFontBold);
        */

    listView->Create(
        WS_EX_CLIENTEDGE,
        WC_LISTVIEW,               // 핵심
        L"",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,             // 테이블 모드
        hwnd,
        (HMENU)3001,
        hInstance
    );

   // listView->SendToHWND(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    listView->SetColumns({ L"ID", L"Name", L"Age" });   
    for (int i = 0; i < 10; ++i)
    {
        std::wstring id = std::to_wstring(i + 1);
        listView->AddRow({ id.c_str(), L"Test", L"10" });
    }
}
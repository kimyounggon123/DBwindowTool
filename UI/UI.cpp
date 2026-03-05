#include "UI.h"

void TableUI::SetColumns(const std::vector<std::wstring>& cols)
{
    // column 생성은 모드 관계없이 수행
    columns = cols;
    LVCOLUMN col{};
    col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    for (int i = 0; i < cols.size(); ++i)
    {
        col.pszText = (LPWSTR)cols[i].c_str();
        col.cx = 120;
        col.iSubItem = i;
        SendMessageW(winUI, LVM_INSERTCOLUMNW, i, (LPARAM)&col);
    }
}
void TableUI::AddRow(const std::vector<std::wstring>& data)
{
    if (data.size() != columns.size()) return;
    tableData.push_back(data);

    if (!useVirtualTable) // 일반 모드일 경우의 출력과정
    {
        int index = SendMessageW(winUI, LVM_GETITEMCOUNT, 0, 0); // 현재 몇 개의 행이 있는가?

        LVITEM item{};
        item.mask = LVIF_TEXT;
        item.iItem = index;
        item.pszText = (LPWSTR)data[0].c_str();

        SendMessageW(winUI, LVM_INSERTITEM, 0, (LPARAM)&item);
        for (int i = 1; i < data.size(); ++i)
        {
            LVITEM sub{};
            sub.iSubItem = i;
            sub.pszText = (LPWSTR)data[i].c_str();

            SendMessageW(winUI, LVM_SETITEMTEXT, index, (LPARAM)&sub);
        }
    }
}
void TableUI::SetItemCount()
{
    ListView_SetItemCount(winUI, tableData.size()); 
}
void TableUI::Clear()
{
    columns.clear();
    tableData.clear();

    

    int colCount = Header_GetItemCount(ListView_GetHeader(winUI));
    for (int i = colCount - 1; i >= 0; --i)
        SendMessageW(winUI, LVM_DELETECOLUMN, i, 0); // 앞에서부터 삭제 시 인덱스 꼬이므로 뒤에서부터

    SendMessageW(winUI, LVM_DELETEALLITEMS, 0, 0);
 
}

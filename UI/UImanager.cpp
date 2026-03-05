#include "UImanager.h"

UImanager* UImanager::instance = nullptr;

bool UImanager::AddUI(const std::wstring& imagePath, const Position& pos)
{
    UIElementPTR ui = std::make_unique<UIElement>(imagePath, pos);
    return AddUI(std::move(ui));
}
bool UImanager::AddUI(const std::wstring& imagePath, const Transform2DINT& transform)
{
    UIElementPTR ui = std::make_unique<UIElement>(imagePath, transform);
    return AddUI(std::move(ui));
}

bool UImanager::AddUI(UIElementPTR ui)
{
    if (ui == nullptr) return false;

    ui->SetID(nextID.load());          // UIElement 내부에 ID가 있다면
    UIElement* raw = ui.get();  // map에 넣을 raw pointer 확보

    uiList.push_back(std::move(ui));  // 소유권은 vector가 가짐
    uiMap.emplace(nextID, raw);           // map은 주소만 저장

    nextID.fetch_add(1);

    return true;
}
bool UImanager::RemoveUI(int ID)
{
    auto mapIt = uiMap.find(ID); // 우선 map의 데이터를 가져옴
    if (mapIt == uiMap.end()) return false;

    UIElement* target = mapIt->second; // raw data

    uiMap.erase(mapIt); // map data부터 삭제

    // 이후 target과 같은 ptr를 가진 list element를 삭제
    uiList.erase(
        std::remove_if(uiList.begin(), uiList.end(),
            [target](const UIElementPTR& ptr)
            {
                return ptr.get() == target;
            }),
        uiList.end());

    return true;
}

bool UImanager::RemoveUI(UIElement* element)
{
    if (element == nullptr) return false;
    return RemoveUI(element->GetID());
}


void UImanager::DeleteAll()
{
    uiMap.clear();   // lookup 먼저 정리
    uiList.clear();  // unique_ptr 전부 파괴 → 자동 delete
    nextID = 0;
}

bool UImanager::Initialize()
{
    bool result = true;
    std::wstring localDict = L"..\\Resources";
    //result = result && AddUI(L"", Transform2DINT{ {0, 0}, {10, 10} });

    return result;
}

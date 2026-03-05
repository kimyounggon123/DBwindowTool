#ifndef _UIMANAGER_H
#define _UIMANAGER_H

#include <unordered_map>
#include "UI.h"

using UIElementPTR = std::unique_ptr<UIElement>;
class UImanager
{
	std::atomic_int nextID;
	std::vector<UIElementPTR> uiList;
	std::unordered_map<int, UIElement*> uiMap;

	static UImanager* instance;
	UImanager() : nextID(0)
	{
	}

public:
	static UImanager& GetInstance()
	{
		if (instance == nullptr) instance = new UImanager();
		return *instance;
	}
	~UImanager()
	{
		DeleteAll();
	}

	bool AddUI(const std::wstring& imagePath, const Position& pos);
	bool AddUI(const std::wstring& imagePath, const Transform2DINT& transform);

	bool AddUI(UIElementPTR element);

	bool RemoveUI(int ID);
	bool RemoveUI(UIElement* elementPTR);

	bool Initialize();

	UIElement* GetUI(int ID)
	{
		auto it = uiMap.find(ID);
		if (it == uiMap.end())  return nullptr;
		return it->second;
	}

	std::vector<UIElementPTR>::const_iterator GetBegin() const { return uiList.begin(); }
	std::vector<UIElementPTR>::const_iterator GetEnd() const { return uiList.end(); }

	void DeleteAll();

};
#endif
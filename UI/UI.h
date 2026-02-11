#ifndef _UI_H
#define _UI_H

#include "..\stdafx.h"
#include <unordered_map>
#include "..\Transform.h"
#include <string>

class UIElement
{
	int ID;
	bool activated;

	std::wstring imagePath;
	Transform2DINT transform;
	RectINT space;


	std::vector<UIElement*> children;
public:
	UIElement(const std::wstring& imagePath, const Position& pos, const RectINT& space) :
		ID(0),
		activated(true),
		transform(pos), space(space),
		imagePath(imagePath)
	{}
	UIElement(const std::wstring& imagePath, const Transform2DINT& transform, const RectINT& space) :
		ID(0),
		activated(true),
		transform(transform), space(space),
		imagePath(imagePath)
	{}
	virtual ~UIElement() {}


	int GetID() const { return ID; }
	void SetID(int ID) { this->ID = ID; }

	const Transform2DINT& GetTransform() const { return transform; }
	void SetPosition(const Position& pos) { transform.pos = pos; }
	void SetAngle(float angle) { transform.angle = angle; }
	void SetScale(const Vector2& scale) { transform.scale = scale; }

	const std::wstring& GetImage() const { return imagePath; }

	bool IsAble() const { return activated; }
	bool SetAble(bool switchThis) { activated = switchThis; }

	//bool AddChild(UIElement* child);
	//bool RemoveChild(int ID);
};
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

	bool AddUI(const std::wstring& imagePath, const Position& pos, const RectINT& space);
	bool AddUI(const std::wstring& imagePath, const Transform2DINT& transform, const RectINT& space);

	bool AddUI(UIElementPTR element);

	bool RemoveUI(int ID);
	bool RemoveUI(UIElement* elementPTR);

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
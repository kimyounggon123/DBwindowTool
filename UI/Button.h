#ifndef _BUTTON_H
#define _BUTTON_H


#include "UI.h"
#include <functional>
#include <unordered_map>

class Button : public WindowUI
{
	std::function<void()> task;

public:
	Button(const std::wstring& imagePath, const Position& pos, std::function<void()> task = nullptr) :
		WindowUI(imagePath, pos), task(task)
	{}
	Button(const std::wstring& imagePath, const Transform2DINT& transform, std::function<void()> task = nullptr) :
		WindowUI(imagePath, transform), task(task)
	{}

	~Button() = default;

	std::function<void()> GetTask() const { return task; }
	void SetTask(std::function<void()> newTask) { task = newTask; }

};


class ToggleGroup;
class Toggle : public WindowUI
{
	std::function<void()> task;

	bool toggled;
	ToggleGroup* group;

public:
	Toggle(const std::wstring& imagePath, const Position& pos, std::function<void()> task = nullptr, bool toggled = false) :
		WindowUI(imagePath, pos), task(task), toggled(toggled), group(nullptr)
	{}
	Toggle(const std::wstring& imagePath, const Transform2DINT& transform, std::function<void()> task = nullptr, bool toggled = false) :
		WindowUI(imagePath, transform), task(task), toggled(toggled), group(nullptr)
	{}
	~Toggle() = default;

	bool IsToggled() const { return toggled; }
	void SetToggled(bool change) { toggled = change; }

	ToggleGroup* FindGroup() const { return group; }
	void ReplaceGroup(ToggleGroup* where) { group = where; }

	std::function<void()> GetTask() const { return task; }
	void SetTask(std::function<void()> newTask) { task = newTask; }

};


class ToggleGroup
{
	std::unordered_map<int, Toggle*> toggleGroup;
	Toggle* toggledOne;
public:
	ToggleGroup() : toggledOne(nullptr)
	{}
	ToggleGroup(Toggle* toggledOne) : toggledOne(toggledOne)
	{
		toggledOne->SetToggled(true);
		toggleGroup.emplace(toggledOne->GetID(), toggledOne);
	}

	bool AddToggle(Toggle* toggle, bool defaultToggle = false);
	void ChangeToggled(Toggle* changed);
};
#endif

#include "Button.h"

bool ToggleGroup::AddToggle(Toggle* toggle, bool defaultToggle)
{
	if (toggle == nullptr || toggle->FindGroup() == this) return false;
	toggle->ReplaceGroup(this);
	if (defaultToggle) ChangeToggled(toggle);
	return true;
}

void ToggleGroup::ChangeToggled(Toggle* changed)
{
	// 소속이 아니거나 이미 on상태라면
	if (changed == nullptr || changed->FindGroup() != this || toggledOne == changed) return;

	changed->SetToggled(true);
	if (toggledOne) toggledOne->SetToggled(false);
	toggledOne = changed;
}
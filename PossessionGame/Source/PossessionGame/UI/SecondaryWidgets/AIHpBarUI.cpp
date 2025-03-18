#include "AIHpBarUI.h"
#include "Components/ProgressBar.h"

void UAIHpBarUI::UpdateHpBar(const float InCurrentHp, const float InMaxHp) const
{
	if (HpBar) HpBar->SetPercent(InCurrentHp / InMaxHp);
}

#include "AIHpBarUI.h"
#include "Components/ProgressBar.h"

void UAIHpBarUI::UpdateHpBar(const float InCurrentHp, const float InMaxHp) const
{
	HpBar->SetPercent(InCurrentHp / InMaxHp);
}

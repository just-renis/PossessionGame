#include "AIHpBarUI.h"
#include "Components/ProgressBar.h"

void UAIHpBarUI::UpdateHpBar(const float InCurrentHp, const float InMaxHp)
{
	if (!HpBar || !HpWhiteBar) return;
	StepHpPercentage = (HpBar->GetPercent() - InCurrentHp / InMaxHp) / 10.f;
	HpWhiteBar->SetPercent(HpBar->GetPercent());
	HpBar->SetPercent(InCurrentHp / InMaxHp);
	GetWorld()->GetTimerManager().SetTimer(UpdateWhiteHpBarHandle, this, &UAIHpBarUI::UpdateWhiteHpBar, 0.02f, true);
}

void UAIHpBarUI::UpdateWhiteHpBar()
{
	if (!HpWhiteBar || !HpBar) return;
	float NewPercentage = HpWhiteBar->GetPercent() - StepHpPercentage;
	if (NewPercentage <= HpBar->GetPercent())
	{
		NewPercentage = HpBar->GetPercent();
		GetWorld()->GetTimerManager().ClearTimer(UpdateWhiteHpBarHandle);
	}
	HpWhiteBar->SetPercent(NewPercentage);
}

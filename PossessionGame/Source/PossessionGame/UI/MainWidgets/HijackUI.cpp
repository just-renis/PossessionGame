#include "HijackUI.h"
#include "Components/Image.h"

void UHijackUI::TriggerHijackProgress() { GetWorld()->GetTimerManager().SetTimer(HijackHandle, this, &UHijackUI::HijackProgress, 0.02f, true); }

void UHijackUI::OpenUI(const float InHijackAnimSpeed)
{
	HijackInv->SetRenderOpacity(1.f);
	PlayAnimation(HijackInitiateAnim, InHijackAnimSpeed);
	HijackSpeedMultiplier = InHijackAnimSpeed;
	TriggerHijackProgress();
}

void UHijackUI::CloseUI() const { HijackInv->SetRenderOpacity(0.f); }

void UHijackUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (const TObjectPtr<UMaterialInterface> Material = Cast<UMaterialInterface>(HijackRadial->GetBrush().GetResourceObject()))
	{
		MI_HijackRadial = UMaterialInstanceDynamic::Create(Material, this);
		FSlateBrush Brush = HijackRadial->GetBrush();
		Brush.SetResourceObject(MI_HijackRadial);
		HijackRadial->SetBrush(Brush);
	}
}

void UHijackUI::HijackProgress()
{
	if (!MI_HijackRadial) return;
	Progress += 0.04f * HijackSpeedMultiplier;
	MI_HijackRadial->SetScalarParameterValue(FName("Progress"), Progress);
	if (Progress >= 1.f)
	{
		GetWorld()->GetTimerManager().ClearTimer(HijackHandle);
		Progress = 0.f;
		MI_HijackRadial->SetScalarParameterValue(FName("Progress"), 0.f);
	}
}

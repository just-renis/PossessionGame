#include "AbilitiesPanelUI.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "PossessionGame/Enums/EUIBar.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

void UAbilitiesPanelUI::SetOverlaysVisibility(const bool bState) const
{
	if (FirstAbilityOverlay) FirstAbilityOverlay->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	if (SecondAbilityOverlay) SecondAbilityOverlay->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	if (ThirdAbilityOverlay) ThirdAbilityOverlay->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UAbilitiesPanelUI::CollapseAbilityIcon(const int32 Index) const
{
	switch (Index)
	{
		case 0:
			if (!FirstAbilityImage || !FirstAbilityProgressBar) return;
			FirstAbilityImage->SetVisibility(ESlateVisibility::Collapsed);
			FirstAbilityProgressBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			MI_AbilityRadials[0]->SetScalarParameterValue("Progress", 0.f);
			return;
		case 1:
			if (!SecondAbilityImage || !SecondAbilityProgressBar) return;
			SecondAbilityImage->SetVisibility(ESlateVisibility::Collapsed);
			SecondAbilityProgressBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			MI_AbilityRadials[1]->SetScalarParameterValue("Progress", 0.f);
			return;
		default:
			if (!ThirdAbilityImage || !ThirdAbilityProgressBar) return;
			ThirdAbilityImage->SetVisibility(ESlateVisibility::Collapsed);
			ThirdAbilityProgressBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			MI_AbilityRadials[2]->SetScalarParameterValue("Progress", 0.f);
	}
}

void UAbilitiesPanelUI::TriggerAbilityIconReady(const EUIBar InBar, const float InCurrent, const float InMax) const
{
	switch (InBar)
	{
		case EUIBar::FirstAbility: 
			if (!FirstAbilityImage || !FirstAbilityProgressBar) return;
			MI_AbilityRadials[0]->SetScalarParameterValue("Progress", InCurrent / InMax);
			if (InCurrent >= InMax)
			{
				FirstAbilityImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				FirstAbilityProgressBar->SetVisibility(ESlateVisibility::Collapsed);
			}
			break;
		case EUIBar::SecondAbility:
			if (!SecondAbilityImage || !SecondAbilityProgressBar) return;
			MI_AbilityRadials[1]->SetScalarParameterValue("Progress", InCurrent / InMax);
			if (InCurrent >= InMax)
			{
				SecondAbilityImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				SecondAbilityProgressBar->SetVisibility(ESlateVisibility::Collapsed);
			}
			break;
		default:
			if (!ThirdAbilityImage || !ThirdAbilityProgressBar) return;
			MI_AbilityRadials[2]->SetScalarParameterValue("Progress", InCurrent / InMax);
			if (InCurrent >= InMax)
			{
				ThirdAbilityImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				ThirdAbilityProgressBar->SetVisibility(ESlateVisibility::Collapsed);
			}
	}
}

void UAbilitiesPanelUI::SetAsCurrent()
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnAbilityUse.BindUObject(this, &UAbilitiesPanelUI::PlayAbilityUseAnim);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnAbilityEnd.BindUObject(this, &UAbilitiesPanelUI::PlayAbilityEndAnim);
}

void UAbilitiesPanelUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetAbilitySlot(AbilityIcon1, FirstAbilityImage, FirstAbilityOverlay);
	SetAbilitySlot(AbilityIcon2, SecondAbilityImage, SecondAbilityOverlay);
	SetAbilitySlot(AbilityIcon3, ThirdAbilityImage, ThirdAbilityOverlay);
}

void UAbilitiesPanelUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (!FirstAbilityProgressBar || !SecondAbilityProgressBar || !ThirdAbilityProgressBar) return;
	if (const TObjectPtr<UMaterialInterface> Material = Cast<UMaterialInterface>(FirstAbilityProgressBar->GetBrush().GetResourceObject()))
	{
		MI_AbilityRadials.Add(UMaterialInstanceDynamic::Create(Material, this));
		FirstAbilityProgressBar->SetBrushFromMaterial(MI_AbilityRadials[0]);
	}
	if (const TObjectPtr<UMaterialInterface> Material = Cast<UMaterialInterface>(SecondAbilityProgressBar->GetBrush().GetResourceObject()))
	{
		MI_AbilityRadials.Add(UMaterialInstanceDynamic::Create(Material, this));
		SecondAbilityProgressBar->SetBrushFromMaterial(MI_AbilityRadials[1]);
	}
	if (const TObjectPtr<UMaterialInterface> Material = Cast<UMaterialInterface>(ThirdAbilityProgressBar->GetBrush().GetResourceObject()))
	{
		MI_AbilityRadials.Add(UMaterialInstanceDynamic::Create(Material, this));
		ThirdAbilityProgressBar->SetBrushFromMaterial(MI_AbilityRadials[2]);
	}
}

void UAbilitiesPanelUI::NativeDestruct()
{
	if (!GetWorld()) return;
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	for (FTimerHandle& Handle : TimerHandles)
	{
		if (TimerManager.IsTimerActive(Handle)) TimerManager.ClearTimer(Handle);
	}
	TimerHandles.Empty();
	Super::NativeDestruct();
}

void UAbilitiesPanelUI::SetAbilitySlot(const TObjectPtr<UTexture2D>& InIcon, const TObjectPtr<UImage>& InImage, const TObjectPtr<UOverlay>& InOverlay)
{
	if (InIcon && InImage) InImage->SetBrushFromAtlasInterface(InIcon);
	else
	{
		InOverlay->SetRenderOpacity(0.8f);
		InOverlay->SetRenderScale(FVector2D(0.8f, 0.8f));
	}
}

void UAbilitiesPanelUI::PlayAbilityUseAnim(const int32 InAbilityIndex, const bool bPlayReverseAfterwards)
{
	PlayAbilityAnim(InAbilityIndex, true);
	if (!bPlayReverseAfterwards) return;
	FTimerDelegate TimerDel;
	TimerDel.BindUFunction(this, "PlayAbilityEndAnim", InAbilityIndex);
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, Ability1UseAnim->GetEndTime(), false);
	TimerHandles.Add(TimerHandle);
}

void UAbilitiesPanelUI::PlayAbilityAnim(const int32 InAbilityIndex, const bool bState)
{
	switch (InAbilityIndex)
	{
		case 0: if (Ability1UseAnim) bState ? PlayAnimation(Ability1UseAnim) : PlayAnimation(Ability1UseAnim, 0.f, 1, EUMGSequencePlayMode::Reverse); break;
		case 1: if (Ability2UseAnim) bState ? PlayAnimation(Ability2UseAnim) : PlayAnimation(Ability2UseAnim, 0.f, 1, EUMGSequencePlayMode::Reverse); break;
		case 2: if (Ability3UseAnim) bState ? PlayAnimation(Ability3UseAnim) : PlayAnimation(Ability3UseAnim, 0.f, 1, EUMGSequencePlayMode::Reverse); break;
		default: ;
	}
}

void UAbilitiesPanelUI::PlayAbilityEndAnim(const int32 InAbilityIndex)
{
	CollapseAbilityIcon(InAbilityIndex);
	PlayAbilityAnim(InAbilityIndex, false);
}

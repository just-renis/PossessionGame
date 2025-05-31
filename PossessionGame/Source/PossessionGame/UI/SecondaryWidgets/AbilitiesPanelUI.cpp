#include "AbilitiesPanelUI.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/GlobalSettingsManager.h"

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
		case 0: if (FirstAbilityImage) FirstAbilityImage->SetVisibility(ESlateVisibility::Collapsed); break;
		case 1: if (SecondAbilityImage) SecondAbilityImage->SetVisibility(ESlateVisibility::Collapsed); break;
		default: if (ThirdAbilityImage) ThirdAbilityImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UAbilitiesPanelUI::UpdateAbilityUI(const TObjectPtr<UAbility>& InAbility)
{
	switch (InAbility->GetAbilityIndex())
	{
		case 0: 
			if (!FirstAbilityImage || !FirstChargesText || !MI_FirstAbilityRadial) return;
			if (InAbility->GetIsMaxCharged())
			{
				if (!bFirstAbilityFullyCharged)
				{
					bFirstAbilityFullyCharged = true;
					MI_FirstAbilityRadial->SetScalarParameterValue("Opacity", 0.f);
					if (FirstAbilityImage->GetVisibility() != ESlateVisibility::SelfHitTestInvisible) FirstAbilityImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
					FirstChargesText->SetText(FText::AsNumber(InAbility->GetCurrentCharges()));
				}
				return;
			}
			if (InAbility->GetCurrentCharges() > 0 && FirstAbilityImage->GetVisibility() != ESlateVisibility::SelfHitTestInvisible) FirstAbilityImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			bFirstAbilityFullyCharged = false;
			MI_FirstAbilityRadial->SetScalarParameterValue("Opacity", 1.f);
			MI_FirstAbilityRadial->SetScalarParameterValue("Progress", InAbility->GetCurrentCooldownSecs() / InAbility->GetReqCooldownSecs());
			FirstChargesText->SetText(FText::AsNumber(InAbility->GetCurrentCharges()));
			break;
		case 1:
			if (!SecondAbilityImage || !SecondChargesText || !MI_SecondAbilityRadial) return;
			if (InAbility->GetIsMaxCharged())
			{
				if (!bSecondAbilityFullyCharged)
				{
					bSecondAbilityFullyCharged = true;
					MI_SecondAbilityRadial->SetScalarParameterValue("Opacity", 0.f);
					if (SecondAbilityImage->GetVisibility() != ESlateVisibility::SelfHitTestInvisible) SecondAbilityImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
					SecondChargesText->SetText(FText::AsNumber(InAbility->GetCurrentCharges()));
				}
				return;
			}
			if (InAbility->GetCurrentCharges() > 0 && SecondAbilityImage->GetVisibility() != ESlateVisibility::SelfHitTestInvisible) SecondAbilityImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			bSecondAbilityFullyCharged = false;
			MI_SecondAbilityRadial->SetScalarParameterValue("Opacity", 1.f);
			MI_SecondAbilityRadial->SetScalarParameterValue("Progress", InAbility->GetCurrentCooldownSecs() / InAbility->GetReqCooldownSecs());
			SecondChargesText->SetText(FText::AsNumber(InAbility->GetCurrentCharges()));
			break;
		default:
			if (!ThirdAbilityImage || !ThirdChargesText || !MI_ThirdAbilityRadial) return;
			if (InAbility->GetIsMaxCharged())
			{
				if (!bThirdAbilityFullyCharged)
				{
					bThirdAbilityFullyCharged = true;
					MI_ThirdAbilityRadial->SetScalarParameterValue("Opacity", 0.f);
					if (ThirdAbilityImage->GetVisibility() != ESlateVisibility::SelfHitTestInvisible) ThirdAbilityImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
					ThirdChargesText->SetText(FText::AsNumber(InAbility->GetCurrentCharges()));
				}
				return;
			}
			if (InAbility->GetCurrentCharges() > 0 && ThirdAbilityImage->GetVisibility() != ESlateVisibility::SelfHitTestInvisible) ThirdAbilityImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			bThirdAbilityFullyCharged = false;
			MI_ThirdAbilityRadial->SetScalarParameterValue("Opacity", 1.f);
			MI_ThirdAbilityRadial->SetScalarParameterValue("Progress", InAbility->GetCurrentCooldownSecs() / InAbility->GetReqCooldownSecs());
			ThirdChargesText->SetText(FText::AsNumber(InAbility->GetCurrentCharges()));
	}
}

void UAbilitiesPanelUI::SetAsCurrent()
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnAbilityUse.BindUObject(this, &UAbilitiesPanelUI::PlayAbilityUseAnim);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnAbilityEnd.BindUObject(this, &UAbilitiesPanelUI::PlayAbilityEndAnim);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnKeyBindChange.AddUObject(this, &UAbilitiesPanelUI::UpdateAbilityKeyBindText);
}

void UAbilitiesPanelUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetAbilitySlot(0, AbilityIcon1, FirstAbilityImage, FirstAbilityOverlay);
	SetAbilitySlot(1, AbilityIcon2, SecondAbilityImage, SecondAbilityOverlay);
	SetAbilitySlot(2, AbilityIcon3, ThirdAbilityImage, ThirdAbilityOverlay);
}

void UAbilitiesPanelUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (!FirstAbilityProgressBar || !SecondAbilityProgressBar || !ThirdAbilityProgressBar || !MI_RadialProgress) return;
	MI_FirstAbilityRadial = UMaterialInstanceDynamic::Create(MI_RadialProgress, this);
	FirstAbilityProgressBar->SetBrushFromMaterial(MI_FirstAbilityRadial);
	MI_SecondAbilityRadial = UMaterialInstanceDynamic::Create(MI_RadialProgress, this);
	SecondAbilityProgressBar->SetBrushFromMaterial(MI_SecondAbilityRadial);
	MI_ThirdAbilityRadial = UMaterialInstanceDynamic::Create(MI_RadialProgress, this);
	ThirdAbilityProgressBar->SetBrushFromMaterial(MI_ThirdAbilityRadial);
}

void UAbilitiesPanelUI::SetAbilitySlot(const int32 InAbilityIndex, const TObjectPtr<UTexture2D>& InIcon, const TObjectPtr<UImage>& InImage, const TObjectPtr<UOverlay>& InOverlay) const
{
	if (InIcon && InImage) InImage->SetBrushFromAtlasInterface(InIcon);
	else InOverlay->SetRenderOpacity(0.8f);
	const FName KeyBindName = InAbilityIndex == 0 ? "FirstAbility" : InAbilityIndex == 1 ? "SecondAbility" : "ThirdAbility";
	if (GetGameInstance()) UpdateAbilityKeyBindText(KeyBindName, GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetKeyBindText(KeyBindName));
}

void UAbilitiesPanelUI::PlayAbilityUseAnim(const int32 InAbilityIndex, const bool bIsCharged, const bool bPlayReverseAfterwards)
{
	PlayAbilityAnim(InAbilityIndex, true);
	if (!bPlayReverseAfterwards) return;
	FTimerDelegate TimerDel;
	TimerDel.BindLambda([this, InAbilityIndex, bIsCharged] { PlayAbilityEndAnim(InAbilityIndex, bIsCharged); });
	switch (InAbilityIndex)
	{
		case 0: GetWorld()->GetTimerManager().SetTimer(FirstAbilityHandle, TimerDel, Ability1UseAnim->GetEndTime(), false); break;
		case 1: GetWorld()->GetTimerManager().SetTimer(SecondAbilityHandle, TimerDel, Ability2UseAnim->GetEndTime(), false); break;
		case 2: GetWorld()->GetTimerManager().SetTimer(ThirdAbilityHandle, TimerDel, Ability3UseAnim->GetEndTime(), false);
		default: ;
	}
}

void UAbilitiesPanelUI::PlayAbilityAnim(const int32 InAbilityIndex, const bool bState)
{
	switch (InAbilityIndex)
	{
		case 0: PlayAnimation(Ability1UseAnim, 0.f, 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse); break;
		case 1: PlayAnimation(Ability2UseAnim, 0.f, 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse); break;
		case 2: PlayAnimation(Ability3UseAnim, 0.f, 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
		default: ;
	}
}

void UAbilitiesPanelUI::UpdateAbilityKeyBindText(const FName& InKeyBindName, const FText& InKeyName) const
{
	if (FirstKeyBindText && InKeyBindName.IsEqual("FirstAbility")) FirstKeyBindText->SetText(InKeyName);
	else if (SecondKeyBindText && InKeyBindName.IsEqual("SecondAbility")) SecondKeyBindText->SetText(InKeyName);
	else if (ThirdKeyBindText && InKeyBindName.IsEqual("ThirdAbility")) ThirdKeyBindText->SetText(InKeyName);
}

void UAbilitiesPanelUI::PlayAbilityEndAnim(const int32 InAbilityIndex, const bool bIsCharged)
{
	if (!bIsCharged) CollapseAbilityIcon(InAbilityIndex);
	PlayAbilityAnim(InAbilityIndex, false);
	switch (InAbilityIndex)
	{
		case 0: GetWorld()->GetTimerManager().ClearTimer(FirstAbilityHandle); break;
		case 1: GetWorld()->GetTimerManager().ClearTimer(SecondAbilityHandle); break;
		case 2: GetWorld()->GetTimerManager().ClearTimer(ThirdAbilityHandle);
		default: ;
	}
}

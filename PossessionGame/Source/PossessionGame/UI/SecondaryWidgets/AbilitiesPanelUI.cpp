#include "AbilitiesPanelUI.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "PossessionGame/Abilities/Management/Ability.h"
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
			if (!FirstAbilityImage || !FirstAbilityProgressBar || !MI_FirstAbilityRadial) return;
			FirstAbilityImage->SetVisibility(ESlateVisibility::Collapsed);
			FirstAbilityProgressBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			MI_FirstAbilityRadial->SetScalarParameterValue("Progress", 0.f);
			return;
		case 1:
			if (!SecondAbilityImage || !SecondAbilityProgressBar || !MI_SecondAbilityRadial) return;
			SecondAbilityImage->SetVisibility(ESlateVisibility::Collapsed);
			SecondAbilityProgressBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			MI_SecondAbilityRadial->SetScalarParameterValue("Progress", 0.f);
			return;
		default:
			if (!ThirdAbilityImage || !ThirdAbilityProgressBar || !MI_ThirdAbilityRadial) return;
			ThirdAbilityImage->SetVisibility(ESlateVisibility::Collapsed);
			ThirdAbilityProgressBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			MI_ThirdAbilityRadial->SetScalarParameterValue("Progress", 0.f);
	}
}

void UAbilitiesPanelUI::UpdateAbilityUI(const TObjectPtr<UAbility>& InAbility) const
{
	switch (InAbility->GetAbilityIndex())
	{
		case 0: 
			if (!FirstAbilityImage || !FirstAbilityProgressBar || !MI_FirstAbilityRadial) return;
			MI_FirstAbilityRadial->SetScalarParameterValue("Progress", InAbility->GetCurrentCooldownSecs() / InAbility->GetReqCooldownSecs());
			if (InAbility->GetIsAbilityCharged())
			{
				if (FirstAbilityImage->GetVisibility() != ESlateVisibility::SelfHitTestInvisible) FirstAbilityImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				if (InAbility->GetIsMaxCharged() && FirstAbilityProgressBar->GetVisibility() != ESlateVisibility::Collapsed) FirstAbilityProgressBar->SetVisibility(ESlateVisibility::Collapsed);
				else if (FirstAbilityProgressBar->GetVisibility() != ESlateVisibility::SelfHitTestInvisible) FirstAbilityProgressBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
			break;
		case 1:
			if (!SecondAbilityImage || !SecondAbilityProgressBar || !MI_SecondAbilityRadial) return;
			MI_SecondAbilityRadial->SetScalarParameterValue("Progress", InAbility->GetCurrentCooldownSecs() / InAbility->GetReqCooldownSecs());
			if (InAbility->GetIsAbilityCharged())
			{
				if (SecondAbilityImage->GetVisibility() != ESlateVisibility::SelfHitTestInvisible) SecondAbilityImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				if (InAbility->GetIsMaxCharged() && SecondAbilityProgressBar->GetVisibility() != ESlateVisibility::Collapsed) SecondAbilityProgressBar->SetVisibility(ESlateVisibility::Collapsed);
				else if (SecondAbilityProgressBar->GetVisibility() != ESlateVisibility::SelfHitTestInvisible) SecondAbilityProgressBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
			break;
		default:
			if (!ThirdAbilityImage || !ThirdAbilityProgressBar || !MI_ThirdAbilityRadial) return;
			MI_ThirdAbilityRadial->SetScalarParameterValue("Progress", InAbility->GetCurrentCooldownSecs() / InAbility->GetReqCooldownSecs());
			if (InAbility->GetIsAbilityCharged())
			{
				if (ThirdAbilityImage->GetVisibility() != ESlateVisibility::SelfHitTestInvisible) ThirdAbilityImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				if (InAbility->GetIsMaxCharged() && ThirdAbilityProgressBar->GetVisibility() != ESlateVisibility::Collapsed) ThirdAbilityProgressBar->SetVisibility(ESlateVisibility::Collapsed);
				else if (ThirdAbilityProgressBar->GetVisibility() != ESlateVisibility::SelfHitTestInvisible) ThirdAbilityProgressBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
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
		MI_FirstAbilityRadial = UMaterialInstanceDynamic::Create(Material, this);
		FirstAbilityProgressBar->SetBrushFromMaterial(MI_FirstAbilityRadial);
	}
	if (const TObjectPtr<UMaterialInterface> Material = Cast<UMaterialInterface>(SecondAbilityProgressBar->GetBrush().GetResourceObject()))
	{
		MI_SecondAbilityRadial = UMaterialInstanceDynamic::Create(Material, this);
		SecondAbilityProgressBar->SetBrushFromMaterial(MI_SecondAbilityRadial);
	}
	if (const TObjectPtr<UMaterialInterface> Material = Cast<UMaterialInterface>(ThirdAbilityProgressBar->GetBrush().GetResourceObject()))
	{
		MI_ThirdAbilityRadial = UMaterialInstanceDynamic::Create(Material, this);
		ThirdAbilityProgressBar->SetBrushFromMaterial(MI_ThirdAbilityRadial);
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

void UAbilitiesPanelUI::PlayAbilityUseAnim(const int32 InAbilityIndex, const bool bIsCharged, const bool bPlayReverseAfterwards)
{
	PlayAbilityAnim(InAbilityIndex, true);
	if (!bPlayReverseAfterwards) return;
	FTimerDelegate TimerDel;
	TimerDel.BindUFunction(this, "PlayAbilityEndAnim", InAbilityIndex, bIsCharged);
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, Ability1UseAnim->GetEndTime(), false);
	TimerHandles.Add(TimerHandle);
}

void UAbilitiesPanelUI::PlayAbilityAnim(const int32 InAbilityIndex, const bool bState)
{
	switch (InAbilityIndex)
	{
		case 0: if (Ability1UseAnim) PlayAnimation(Ability1UseAnim, 0.f, 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse); break;
		case 1: if (Ability2UseAnim) PlayAnimation(Ability2UseAnim, 0.f, 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse); break;
		case 2: if (Ability3UseAnim) PlayAnimation(Ability3UseAnim, 0.f, 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse); break;
		default: ;
	}
}

void UAbilitiesPanelUI::PlayAbilityEndAnim(const int32 InAbilityIndex, const bool bIsCharged)
{
	UE_LOG(LogTemp, Warning, TEXT("PlayAbilityEndAnim: %i InAbilityIndex: %i"), bIsCharged, InAbilityIndex);
	if (!bIsCharged) CollapseAbilityIcon(InAbilityIndex);
	PlayAbilityAnim(InAbilityIndex, false);
}

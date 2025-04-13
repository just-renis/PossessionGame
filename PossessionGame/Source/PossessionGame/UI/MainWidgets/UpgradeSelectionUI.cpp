#include "UpgradeSelectionUI.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/UI/SecondaryWidgets/UpgradeCardUI.h"
#include "PossessionGame/UI/SecondaryWidgets/UpgradeDetailsPanelUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/UpgradeManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UUpgradeSelectionUI::OpenUI()
{
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::UpgradePanelAppear);
	(void)DelegateManager->OnUpgradeSelectionOpened.ExecuteIfBound();
	Toggle(true);
	if (UpgradeMenuOpen) PlayAnimation(UpgradeMenuOpen);
	UpgradesData = GetWorld()->GetSubsystem<UUpgradeManager>()->GetRandomUpgrades();
	if (UpgradesData.Num() > 0)
	{
		if (PlayerCardOption1)
		{
			PlayerCardOption1->SetupCard(true, PlayerCardBackgroundTexture, UpgradesData[0], UpgradesData[0]->GetCharacter() == ECharacterType::Ranger ? RangerIconTexture : TricksterIconTexture, PlayerIconTexture);
			PlayerCardOption1->SetUpgradeText(ReceiveUpgradeText(true, UpgradesData[0]));
		}
		if (EnemyCardOption1)
		{
			EnemyCardOption1->SetupCard(false, EnemyCardBackgroundTexture, UpgradesData[0], EnemyIconTexture);
			EnemyCardOption1->SetUpgradeText(ReceiveUpgradeText(false, UpgradesData[0]));
		}
	}
	if (UpgradesData.Num() > 1)
	{
		if (PlayerCardOption2)
		{
			PlayerCardOption2->SetupCard(true, PlayerCardBackgroundTexture, UpgradesData[1], UpgradesData[1]->GetCharacter() == ECharacterType::Ranger ? RangerIconTexture : TricksterIconTexture, PlayerIconTexture);
			PlayerCardOption2->SetUpgradeText(ReceiveUpgradeText(true, UpgradesData[1]));
		}
		if (EnemyCardOption2)
		{
			EnemyCardOption2->SetupCard(false, EnemyCardBackgroundTexture, UpgradesData[1], EnemyIconTexture);
			EnemyCardOption2->SetUpgradeText(ReceiveUpgradeText(false, UpgradesData[1]));
		}
	}
}

void UUpgradeSelectionUI::CloseUI()
{
	if (PlayerCardOption1) PlayerCardOption1->ToggleCardVisibility(false);
	if (EnemyCardOption1) EnemyCardOption1->ToggleCardVisibility(false);
	if (PlayerCardOption2) PlayerCardOption2->ToggleCardVisibility(false);
	if (EnemyCardOption2) EnemyCardOption2->ToggleCardVisibility(false);
	Toggle(false);
}

FString UUpgradeSelectionUI::ConvertUpgradeAttributeValueToString(const float InValue)
{
	FString FloatValueString = FString::SanitizeFloat(InValue);
	if (FloatValueString.EndsWith(TEXT(".0"))) FloatValueString.RemoveFromEnd(TEXT(".0"));
	return FloatValueString;
}

FString UUpgradeSelectionUI::GetUpgradeTextFromUpgrade(const EUpgrade InUpgrade)
{
	switch (InUpgrade)
	{
		case EUpgrade::RangerAbility1: return " GRENADE LAUNCHER UPGRADE";
		case EUpgrade::RangerAbility2: return " SHIELD UPGRADE";
		case EUpgrade::RangerAbility3: return " SLOWMOTION UPGRADE";
		case EUpgrade::TricksterAbility1: return " POISON UPGRADE";
		case EUpgrade::TricksterAbility2: return " EMP UPGRADE";
		case EUpgrade::RangerBaseStat: return " RANGER BASE STATS UPGRADE";
	}
	return "";
}

FString UUpgradeSelectionUI::GetUpgradeTextFromSingularAttributeValue(const TPair<EUpgradeAttribute, float> InAttribute)
{
	switch (InAttribute.Key)
	{
		case EUpgradeAttribute::FlatDamage: return " DAMAGE INCREASED BY " + ConvertUpgradeAttributeValueToString(InAttribute.Value) + " POINTS\n";
		case EUpgradeAttribute::FlatHealth: return " HEALTH INCREASED BY " + ConvertUpgradeAttributeValueToString(InAttribute.Value) + " POINTS\n";
		case EUpgradeAttribute::DamagePercentage: return " DAMAGE INCREASED BY " + ConvertUpgradeAttributeValueToString(InAttribute.Value) + "%\n";
		case EUpgradeAttribute::ProjectilesAmount: return " PROJECTILES INCREASED BY " + ConvertUpgradeAttributeValueToString(InAttribute.Value) + "\n";
		case EUpgradeAttribute::AbilityCooldownReduction: return " COOLDOWN REDUCED BY " + ConvertUpgradeAttributeValueToString(InAttribute.Value) + "%\n";
		case EUpgradeAttribute::Size: return " SIZE INCREASED BY " + ConvertUpgradeAttributeValueToString(InAttribute.Value) + "%\n";
		case EUpgradeAttribute::AbilityDuration: return " DURATION INCREASED BY " + ConvertUpgradeAttributeValueToString(InAttribute.Value) + "%\n";
		case EUpgradeAttribute::EffectDuration: return " EFFECT DURATION INCREASED BY " + ConvertUpgradeAttributeValueToString(InAttribute.Value) + "%\n";
	}
	return "";
}

void UUpgradeSelectionUI::Toggle(const bool bState)
{
	SetVisibility(bState ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	if (bState)
	{
		PlayerController->SetInputMode(FInputModeUIOnly());
		PlayerController->FlushPressedKeys();
	}
	else PlayerController->SetInputMode(FInputModeGameOnly());
	GetWorld()->GetSubsystem<UWidgetManager>()->SetCurrentStateOfUI(bState ? EWidget::UpgradeSelectionUI : EWidget::None);
}

void UUpgradeSelectionUI::NativeConstruct()
{
	Super::NativeConstruct();
	DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	DelegateManager->OnUpgradeSelected.BindUObject(this, &UUpgradeSelectionUI::SetTriggerUpgradeMenuCloseAnimation);
	if (PlayerCardOption1)
	{
		PlayerCardOption1->GetUpgradeButton()->OnHovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerHoverAnimForFirstOption);
		PlayerCardOption1->GetUpgradeButton()->OnUnhovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerUnHoverAnimForFirstOption);
		PlayerCardOption1->GetUpgradeButton()->OnClicked.AddDynamic(this, &UUpgradeSelectionUI::TriggerSelectAnimForFirstOption);
	}
	if (PlayerCardOption2)
	{
		PlayerCardOption2->GetUpgradeButton()->OnHovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerHoverAnimForSecondOption);
		PlayerCardOption2->GetUpgradeButton()->OnUnhovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerUnHoverAnimForSecondOption);
		PlayerCardOption2->GetUpgradeButton()->OnClicked.AddDynamic(this, &UUpgradeSelectionUI::TriggerSelectAnimForSecondOption);
	}
	if (EnemyCardOption1)
	{
		EnemyCardOption1->GetUpgradeButton()->OnHovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerHoverAnimForFirstOption);
		EnemyCardOption1->GetUpgradeButton()->OnUnhovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerUnHoverAnimForFirstOption);
		EnemyCardOption1->GetUpgradeButton()->OnClicked.AddDynamic(this, &UUpgradeSelectionUI::TriggerSelectAnimForFirstOption);
	}
	if (EnemyCardOption2)
	{
		EnemyCardOption2->GetUpgradeButton()->OnHovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerHoverAnimForSecondOption);
		EnemyCardOption2->GetUpgradeButton()->OnUnhovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerUnHoverAnimForSecondOption);
		EnemyCardOption2->GetUpgradeButton()->OnClicked.AddDynamic(this, &UUpgradeSelectionUI::TriggerSelectAnimForSecondOption);
	}
}

void UUpgradeSelectionUI::SetTriggerUpgradeMenuCloseAnimation()
{
	UKismetSystemLibrary::K2_SetTimer(this, "TriggerUpgradeMenuCloseAnimation", 0.5f, false);
}

FString UUpgradeSelectionUI::ReceiveUpgradeText(const bool bPlayerCard, const TObjectPtr<UUpgradeData>& InUpgradeData, const bool bIsForDetailsPanel)
{
	if (bPlayerCard && !bIsForDetailsPanel) return GetUpgradeTextFromUpgrade(InUpgradeData->GetUpgradeName());
	FString UpgradeTextString = "";
	const TObjectPtr<USaveSlot> SaveSlot = Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot();
	const int32 NextLevel = SaveSlot->GetLevelOfAnUpgrade(InUpgradeData->GetUpgradeName()) + 1;
	FLevelUpgradeData CurrentUpgradeData = InUpgradeData->GetAttributesForLevel(bPlayerCard, NextLevel);
	if (NextLevel > 1) ProcessCurrentUpgradeData(bPlayerCard, NextLevel - 1, InUpgradeData, CurrentUpgradeData);
	for (const TPair<EUpgradeAttribute, float> Attribute : CurrentUpgradeData.Upgrades) UpgradeTextString += GetUpgradeTextFromSingularAttributeValue(Attribute);
	UpgradeTextString.RemoveFromEnd(TEXT("\n"));
	return UpgradeTextString;
}

void UUpgradeSelectionUI::ProcessCurrentUpgradeData(const bool bPlayerCard, const int32 PreviousLevel, const TObjectPtr<UUpgradeData>& InUpgradeData, FLevelUpgradeData& CurrentUpgradeData)
{
	TArray<EUpgradeAttribute> KeysToRemove;
	FLevelUpgradeData PreviousUpgradeData = InUpgradeData->GetAttributesForLevel(bPlayerCard, PreviousLevel);
	for (TPair<EUpgradeAttribute, float>& CurrPair : CurrentUpgradeData.Upgrades)
	{
		if (float* PrevValue = PreviousUpgradeData.Upgrades.Find(CurrPair.Key))
		{
			CurrPair.Value -= *PrevValue;
			if (FMath::IsNearlyEqual(CurrPair.Value, 0.f, 0.001f)) KeysToRemove.Add(CurrPair.Key);
		}
	}
	for (const EUpgradeAttribute Key : KeysToRemove) CurrentUpgradeData.Upgrades.Remove(Key);
}

void UUpgradeSelectionUI::FinishedUpgradeSelectionCloseAnimation() const
{
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UUpgradeSelectionUI>(EWidget::UpgradeSelectionUI)->CloseUI();
	(void)DelegateManager->OnUpgradeSelectionFinished.ExecuteIfBound();
}

void UUpgradeSelectionUI::TriggerHoverAnimForFirstOption()
{
	if (PlayerCardOption1Popup && EnemyCardOption1Popup)
	{
		GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::UpgradeCardHover);
		StopAnimation(PlayerCardOption1UnHover);
		StopAnimation(EnemyCardOption1UnHover);
		PlayAnimation(PlayerCardOption1Popup);
		PlayAnimation(EnemyCardOption1Popup);
		if (!UpgradeDetailsPanel) return;
		if (UpgradesData.Num() > 0)
		{
			UpgradeDetailsPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			UpgradeDetailsPanel->SetUpgradeText(ReceiveUpgradeText(true, UpgradesData[0], true));
		}
	}
}

void UUpgradeSelectionUI::TriggerUnHoverAnimForFirstOption()
{
	if (PlayerCardOption1UnHover && EnemyCardOption1UnHover)
	{
		StopAnimation(PlayerCardOption1Popup);
		StopAnimation(EnemyCardOption1Popup);
		PlayAnimation(PlayerCardOption1UnHover);
		PlayAnimation(EnemyCardOption1UnHover);
		if (UpgradeDetailsPanel) UpgradeDetailsPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UUpgradeSelectionUI::TriggerHoverAnimForSecondOption()
{
	if (PlayerCardOption2Popup && EnemyCardOption2Popup)
	{
		GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::UpgradeCardHover);
		StopAnimation(PlayerCardOption2UnHover);
		StopAnimation(EnemyCardOption2UnHover);
		PlayAnimation(PlayerCardOption2Popup);
		PlayAnimation(EnemyCardOption2Popup);
		if (!UpgradeDetailsPanel) return;
		if (UpgradesData.Num() > 1)
		{
			UpgradeDetailsPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			UpgradeDetailsPanel->SetUpgradeText(ReceiveUpgradeText(true, UpgradesData[1], true));
		}
	}
}

void UUpgradeSelectionUI::TriggerUnHoverAnimForSecondOption()
{
	if (PlayerCardOption2UnHover && EnemyCardOption2UnHover)
	{
		StopAnimation(PlayerCardOption2Popup);
		StopAnimation(EnemyCardOption2Popup);
		PlayAnimation(PlayerCardOption2UnHover);
		PlayAnimation(EnemyCardOption2UnHover);
		if (UpgradeDetailsPanel) UpgradeDetailsPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UUpgradeSelectionUI::TriggerSelectAnimForFirstOption()
{
	if (PlayerCardOption1Select && EnemyCardOption1Select)
	{
		PlayAnimation(PlayerCardOption1Select);
		PlayAnimation(EnemyCardOption1Select);
	}
}

void UUpgradeSelectionUI::TriggerSelectAnimForSecondOption()
{
	if (PlayerCardOption2Select && EnemyCardOption2Select)
	{
		PlayAnimation(PlayerCardOption2Select);
		PlayAnimation(EnemyCardOption2Select);
	}
}

void UUpgradeSelectionUI::TriggerUpgradeMenuCloseAnimation()
{
	if (UpgradeMenuClose)
	{
		PlayAnimation(UpgradeMenuClose);
		UKismetSystemLibrary::K2_SetTimer(this, "FinishedUpgradeSelectionCloseAnimation", UpgradeMenuClose->GetEndTime(), false);
	}
}

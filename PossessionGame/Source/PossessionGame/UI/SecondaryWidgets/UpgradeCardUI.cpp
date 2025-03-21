#include "UpgradeCardUI.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

void UUpgradeCardUI::SetupCard(const bool bPlayerCard, const TObjectPtr<UTexture2D>& InCardBackgroundImage, const TObjectPtr<UUpgradeData>& InData, const TObjectPtr<UTexture2D>& InCharacterIcon, const TObjectPtr<UTexture2D>& InPlayerIcon)
{
	Data = InData;
	ToggleCardVisibility(true);
	SetCardBackgroundBorderImage(InCardBackgroundImage);
	SetCharacterIcon(InCharacterIcon);
	SetPlayerIcon(InPlayerIcon);
	SetCardIcon(bPlayerCard ? Data->GetPlayerCardIcon() : Data->GetEnemyCardIcon());
	SetUpgradeText(bPlayerCard);
	if (bPlayerCard) SetCharacterText(FText::FromString(UEnum::GetValueAsString(Data->GetCharacter()).Replace(TEXT("ECharacterType::"), TEXT("")).ToUpper()), Data->GetCharacter() == ECharacterType::Ranger ? FColor::Red : FColor::Cyan);
	else
	{
		SetCharacterText(FText::FromString("ENEMY"), FColor::Red);
		SetPlayerOrEnemyIconInvisible();
	}
}

void UUpgradeCardUI::ToggleCardVisibility(const bool bState)
{
	SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	if (bState && UpgradeButton) UpgradeButton->SetIsEnabled(bState);
}

FString UUpgradeCardUI::ConvertUpgradeAttributeValueToString(const float InValue)
{
	FString FloatValueString = FString::SanitizeFloat(InValue);
	if (FloatValueString.EndsWith(TEXT(".0"))) FloatValueString.RemoveFromEnd(TEXT(".0"));
	return FloatValueString;
}

FString UUpgradeCardUI::GetUpgradeTextFromSingularAttributeValue(const TPair<EUpgradeAttribute, float> InAttribute)
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

void UUpgradeCardUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (UpgradeButton) UpgradeButton->OnClicked.AddDynamic(this, &UUpgradeCardUI::SelectUpgrade);
}

void UUpgradeCardUI::SetCardBackgroundBorderImage(const TObjectPtr<UTexture2D>& InImage) const { if (CardBackgroundBorder && InImage) CardBackgroundBorder->SetBrushFromTexture(InImage); }

void UUpgradeCardUI::SetCharacterIcon(const TObjectPtr<UTexture2D>& InImage) const { if (CharacterIcon && InImage) CharacterIcon->SetBrushFromTexture(InImage); }

void UUpgradeCardUI::SetPlayerIcon(const TObjectPtr<UTexture2D>& InImage) const { if (PlayerOrEnemyIcon && InImage) PlayerOrEnemyIcon->SetBrushFromTexture(InImage); }

void UUpgradeCardUI::SetPlayerOrEnemyIconInvisible() const { if (PlayerOrEnemyIcon) PlayerOrEnemyIcon->SetVisibility(ESlateVisibility::Collapsed); }

void UUpgradeCardUI::SetCardIcon(const TObjectPtr<UTexture2D>& InImage) const { if (CardIcon && InImage) CardIcon->SetBrushFromTexture(InImage); }

void UUpgradeCardUI::SetCharacterText(const FText& InText, const FColor InColor) const
{
	if (!CharacterText) return;
	CharacterText->SetText(InText);
	CharacterText->SetColorAndOpacity(InColor);
}

void UUpgradeCardUI::SetUpgradeText(const bool bPlayerCard)
{
	const TObjectPtr<USaveSlot> SaveSlot = Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot();
	const int32 NextLevel = SaveSlot->GetLevelOfAnUpgrade(Data->GetUpgradeName()) + 1;
	FLevelUpgradeData CurrentUpgradeData = Data->GetAttributesForLevel(bPlayerCard, NextLevel);
	if (NextLevel > 1) ProcessCurrentUpgradeData(bPlayerCard, NextLevel - 1, CurrentUpgradeData);
	FString UpgradeTextString = "";
	for (const TPair<EUpgradeAttribute, float> Attribute : CurrentUpgradeData.Upgrades) UpgradeTextString += GetUpgradeTextFromSingularAttributeValue(Attribute);
	UpgradeTextString.RemoveFromEnd(TEXT("\n"));
	if (UpgradeText) UpgradeText->SetText(FText::FromString(UpgradeTextString));
}

void UUpgradeCardUI::ProcessCurrentUpgradeData(const bool bPlayerCard, const int32 PreviousLevel, FLevelUpgradeData& CurrentUpgradeData) const
{
	TArray<EUpgradeAttribute> KeysToRemove;
	FLevelUpgradeData PreviousUpgradeData = Data->GetAttributesForLevel(bPlayerCard, PreviousLevel);
	TMap<EUpgradeAttribute, float>::TIterator PrevIt = PreviousUpgradeData.Upgrades.CreateIterator();
	TMap<EUpgradeAttribute, float>::TIterator CurrIt = CurrentUpgradeData.Upgrades.CreateIterator();
	for (; PrevIt; ++PrevIt, ++CurrIt)
	{
		CurrIt->Value -= PrevIt->Value;
		if (FMath::IsNearlyEqual(CurrIt->Value, 0.f, 0.001f)) KeysToRemove.Add(CurrIt->Key);
	}
	for (const EUpgradeAttribute Key : KeysToRemove) CurrentUpgradeData.Upgrades.Remove(Key);
}

void UUpgradeCardUI::SelectUpgrade()
{
	if (!UpgradeButton && !UpgradeButton->GetIsEnabled()) return;
	UpgradeButton->SetIsEnabled(false);
	const TObjectPtr<USaveSlot> SaveSlot = Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot();
	SaveSlot->AddUpgrade(Data->GetUpgradeName(), SaveSlot->GetLevelOfAnUpgrade(Data->GetUpgradeName()) + 1);
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnUpgradeSelected.ExecuteIfBound();
}

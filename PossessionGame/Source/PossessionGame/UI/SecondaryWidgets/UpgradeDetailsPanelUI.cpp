#include "UpgradeDetailsPanelUI.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/UI/MainWidgets/UpgradeSelectionUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UUpgradeDetailsPanelUI::Setup(const TObjectPtr<UUpgradeData>& InUpgradeData)
{
	UpgradeData = InUpgradeData;
	bHasBeenSelected = false;
	if (UpgradeChoiceLogo) UpgradeChoiceLogo->SetBrushFromAtlasInterface(UpgradeData->GetPlayerCardIcon());
	if (UpgradeChoiceIndexText) UpgradeChoiceIndexText->SetText(FText::AsNumber(ChoiceIndex + 1));
	SetUpgradeText();
	if (UpgradeChoiceButton) UpgradeChoiceButton->SetIsEnabled(true);
}

FString UUpgradeDetailsPanelUI::ConvertUpgradeAttributeValueToString(const float InValue)
{
	FString FloatValueString = FString::SanitizeFloat(InValue);
	if (FloatValueString.EndsWith(TEXT(".0"))) FloatValueString.RemoveFromEnd(TEXT(".0"));
	return FloatValueString;
}

FString UUpgradeDetailsPanelUI::GetUpgradeTextFromSingularAttributeValue(const TPair<EUpgradeAttribute, float> InAttribute)
{
	switch (InAttribute.Key)
	{
		case EUpgradeAttribute::FlatDamage: return "+" + ConvertUpgradeAttributeValueToString(InAttribute.Value) + " DAMAGE\n";
		case EUpgradeAttribute::FlatHealth: return "+" + ConvertUpgradeAttributeValueToString(InAttribute.Value) + " HEALTH\n";
		case EUpgradeAttribute::DamagePercentage: return "INCREASED " + ConvertUpgradeAttributeValueToString(InAttribute.Value) + "% DAMAGE\n";
		case EUpgradeAttribute::ProjectilesAmount: return "+" + ConvertUpgradeAttributeValueToString(InAttribute.Value) + " PROJECTILES\n";
		case EUpgradeAttribute::AbilityCooldownReduction: return "INCREASED " + ConvertUpgradeAttributeValueToString(InAttribute.Value) + "% COOLDOWN\n";
		case EUpgradeAttribute::Size: return "INCREASED " + ConvertUpgradeAttributeValueToString(InAttribute.Value) + "% AREA\n";
		case EUpgradeAttribute::AbilityDuration: return "INCREASED " + ConvertUpgradeAttributeValueToString(InAttribute.Value) + "% ABILITY DURATION\n";
		case EUpgradeAttribute::EffectDuration: return "INCREASED " + ConvertUpgradeAttributeValueToString(InAttribute.Value) + "% EFFECT DURATION\n";
	}
	return "";
}

void UUpgradeDetailsPanelUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (!UpgradeChoiceButton) return;
	UpgradeChoiceButton->OnClicked.AddDynamic(this, &UUpgradeDetailsPanelUI::SelectUpgrade);
	UpgradeChoiceButton->OnHovered.AddDynamic(this, &UUpgradeDetailsPanelUI::PlayHoverAnim);
	UpgradeChoiceButton->OnUnhovered.AddDynamic(this, &UUpgradeDetailsPanelUI::PlayUnHoverAnim);
}

void UUpgradeDetailsPanelUI::ProcessCurrentUpgradeData(FUpgradeAttributes& PreviousUpgradeData, FUpgradeAttributes& CurrentUpgradeData)
{
	TArray<EUpgradeAttribute> KeysToRemove;
	for (TPair<EUpgradeAttribute, float>& CurrPair : CurrentUpgradeData.Upgrades)
	{
		if (const float* PrevValue = PreviousUpgradeData.Upgrades.Find(CurrPair.Key))
		{
			CurrPair.Value -= *PrevValue;
			if (FMath::IsNearlyEqual(CurrPair.Value, 0.f, 0.001f)) KeysToRemove.Add(CurrPair.Key);
		}
	}
	for (const EUpgradeAttribute Key : KeysToRemove) CurrentUpgradeData.Upgrades.Remove(Key);
}

void UUpgradeDetailsPanelUI::SetUpgradeText()
{
	if (!UpgradeText) return;
	FString UpgradeTextString = "";
	const TObjectPtr<USaveSlot> SaveSlot = Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot();
	const int32 NextLevel = SaveSlot->GetLevelOfChoice(UpgradeData->GetUpgradeName(), ChoiceIndex) + 1;
	FUpgradeAttributes CurrentUpgradeData = UpgradeData->GetAttributesForLevel(true, NextLevel, ChoiceIndex);
	if (NextLevel > 1)
	{
		FUpgradeAttributes PreviousUpgradeData = UpgradeData->GetAttributesForLevel(true, NextLevel - 1, ChoiceIndex);
		ProcessCurrentUpgradeData(PreviousUpgradeData, CurrentUpgradeData);
	}
	for (const TPair<EUpgradeAttribute, float> Attribute : CurrentUpgradeData.Upgrades) UpgradeTextString += GetUpgradeTextFromSingularAttributeValue(Attribute);
	UpgradeTextString.RemoveFromEnd(TEXT("\n"));
	UpgradeText->SetText(FText::FromString(UpgradeTextString));
}

void UUpgradeDetailsPanelUI::SelectUpgrade()
{
	bHasBeenSelected = true;
	PlayAnimation(PressAnim);
	UpgradeChoiceButton->SetIsEnabled(false);
	const TObjectPtr<USaveSlot> SaveSlot = Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot();
	SaveSlot->AddUpgrade(UpgradeData->GetUpgradeName(), ChoiceIndex);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UUpgradeSelectionUI>(EWidget::UpgradeSelectionUI)->TriggerUpgradeMenuCloseAnimation();
}

void UUpgradeDetailsPanelUI::PlayHoverAnim()
{
	StopAnimation(UnHoverAnim);
	PlayAnimation(HoverAnim);
}

void UUpgradeDetailsPanelUI::PlayUnHoverAnim()
{
	StopAnimation(HoverAnim);
	PlayAnimation(UnHoverAnim, 0, 1, EUMGSequencePlayMode::Reverse);
}

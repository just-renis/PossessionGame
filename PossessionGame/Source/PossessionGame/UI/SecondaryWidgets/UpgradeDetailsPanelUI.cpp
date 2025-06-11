#include "UpgradeDetailsPanelUI.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/UI/MainWidgets/UpgradeSelectionUI.h"
#include "PossessionGame/Utilities/Data Utilities/RunSaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UUpgradeDetailsPanelUI::SetButtonVisibility(const bool bState) const
{
	if (!UpgradeChoiceButton) return;
	UpgradeChoiceButton->SetVisibility(bState ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	UpgradeChoiceButton->SetIsEnabled(bState);
}

void UUpgradeDetailsPanelUI::Setup(const TObjectPtr<UUpgradeData>& InUpgradeData)
{
	UpgradeData = InUpgradeData;
	bHasBeenSelected = false;
	if (UpgradeDetailsPanelOverlay) UpgradeDetailsPanelOverlay->SetRenderScale(FVector2D(1.f, 1.f));
	if (UpgradeChoiceLogo) UpgradeChoiceLogo->SetBrushFromAtlasInterface(UpgradeData->GetPlayerCardIcon());
	if (UpgradeChoiceIndexText) UpgradeChoiceIndexText->SetText(FText::AsNumber(ChoiceIndex + 1));
	SetUpgradeText();
	SetButtonVisibility(true);
}

void UUpgradeDetailsPanelUI::PlayDisappearAnim()
{
	UpgradeChoiceButton->SetIsEnabled(false);
	if (!bHasBeenSelected) PlayAnimation(DisappearAnim);
}

FString UUpgradeDetailsPanelUI::ConvertUpgradeAttributeValueToString(const float InValue)
{
	FString FloatValueString = FString::SanitizeFloat(InValue);
	if (FloatValueString.EndsWith(TEXT(".0"))) FloatValueString.RemoveFromEnd(TEXT(".0"));
	return FloatValueString;
}

FString UUpgradeDetailsPanelUI::GetUpgradeTextFromSingularAttributeValue(const TPair<EUpgradeAttribute, float> InAttribute)
{
	const FString AttributeValue = ConvertUpgradeAttributeValueToString(InAttribute.Value);
	switch (InAttribute.Key)
	{
		case EUpgradeAttribute::FlatDamage: return "+" + AttributeValue + " DAMAGE\n";
		case EUpgradeAttribute::FlatHealth: return "+" + AttributeValue + " HEALTH\n";
		case EUpgradeAttribute::DamagePercentage: return "INCREASED " + AttributeValue + "% DAMAGE\n";
		case EUpgradeAttribute::ProjectilesAmount: return "+" + AttributeValue + " PROJECTILES\n";
		case EUpgradeAttribute::AbilityCooldownReduction: return "REDUCED " + AttributeValue + "% ABILITY COOLDOWN\n";
		case EUpgradeAttribute::AbilityChargesAmount: return "+" + AttributeValue + " ABILITY CHARGES\n";
		case EUpgradeAttribute::Size: return "INCREASED " + AttributeValue + "% AREA\n";
		case EUpgradeAttribute::AbilityDuration: return "INCREASED " + AttributeValue + "% ABILITY DURATION\n";
		case EUpgradeAttribute::EffectDuration: return "INCREASED " + AttributeValue + "% EFFECT DURATION\n";
		case EUpgradeAttribute::ReduceDamageTaken: return "REDUCED " + AttributeValue + "% DAMAGE TAKEN\n";
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
	const TObjectPtr<URunSaveSlot> RunSaveSlot = Cast<UPossessionGameInstance>(GetGameInstance())->GetRunSaveSlot();
	const int32 NextLevel = RunSaveSlot->GetLevelOfChoice(UpgradeData->GetUpgradeName(), ChoiceIndex) + 1;
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
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::UpgradeCardSelect);
	bHasBeenSelected = true;
	PlayAnimation(PressAnim);
	const TObjectPtr<URunSaveSlot> RunSaveSlot = Cast<UPossessionGameInstance>(GetGameInstance())->GetRunSaveSlot();
	RunSaveSlot->AddUpgrade(Cast<UPossessionGameInstance>(GetGameInstance()), UpgradeData->GetUpgradeName(), ChoiceIndex);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UUpgradeSelectionUI>(EWidget::UpgradeSelectionUI)->TriggerUpgradeMenuCloseAnimation();
}

void UUpgradeDetailsPanelUI::PlayHoverAnim()
{
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ButtonHover);
	StopAnimation(UnHoverAnim);
	PlayAnimation(HoverAnim);
}

void UUpgradeDetailsPanelUI::PlayUnHoverAnim()
{
	StopAnimation(HoverAnim);
	PlayAnimation(UnHoverAnim);
}

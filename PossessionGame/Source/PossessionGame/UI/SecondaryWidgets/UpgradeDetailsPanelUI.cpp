#include "UpgradeDetailsPanelUI.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/UI/MainWidgets/UpgradeSelectionUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UUpgradeDetailsPanelUI::SetButtonVisibility(const bool bState) const
{
	if (!UpgradeChoiceButton) return;
	UpgradeChoiceButton->SetVisibility(bState ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	UpgradeChoiceButton->SetIsEnabled(bState);
}

void UUpgradeDetailsPanelUI::Setup(const EUpgrade InUpgradeName, const FUpgradeChoicePair& InUpgradeChoicePair)
{
	UpgradeName = InUpgradeName;
	UpgradeChoice = InUpgradeChoicePair;
	bHasBeenSelected = false;
	UpgradeDetailsPanelOverlay->SetRenderScale(FVector2D(1.f, 1.f));
	SetUpgradeText(true);
	SetUpgradeText(false);
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
		case EUpgradeAttribute::Special1: return "NIGGA1";
		case EUpgradeAttribute::Special2: return "NIGGA1";
		case EUpgradeAttribute::Special3: return "NIGGA1";
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

void UUpgradeDetailsPanelUI::SetUpgradeText(const bool bIsPlayerUpgrade)
{
	if (UpgradeChoice.IsEmpty())
	{
		UpgradeChoiceButton->SetIsEnabled(false);
		if (bIsPlayerUpgrade) PlayerUpgradeText->SetText(FText::FromString("No available choice"));
		else EnemyUpgradeText->SetText(FText::FromString("No available choice"));
		return;
	}
	FString UpgradeTextString = "";
	FUpgradeAttributes UpgradeAttributesData = bIsPlayerUpgrade ? UpgradeChoice.PlayerAttributes : UpgradeChoice.EnemyAttributes;
	for (const TPair<EUpgradeAttribute, float> Attribute : UpgradeAttributesData.UpgradeAttributes) UpgradeTextString += GetUpgradeTextFromSingularAttributeValue(Attribute);
	UpgradeTextString.RemoveFromEnd(TEXT("\n"));
	if (bIsPlayerUpgrade) PlayerUpgradeText->SetText(FText::FromString(UpgradeTextString));
	else EnemyUpgradeText->SetText(FText::FromString(UpgradeTextString));
}

void UUpgradeDetailsPanelUI::SelectUpgrade()
{
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::UpgradeCardSelect);
	bHasBeenSelected = true;
	PlayAnimation(PressAnim);
	GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->AddUpgrade(Cast<UPossessionGameInstance>(GetGameInstance()), UpgradeName, UpgradeChoice);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UUpgradeSelectionUI>(EWidget::UpgradeSelectionUI)->TriggerUpgradeMenuCloseAnimation();
}

void UUpgradeDetailsPanelUI::PlayHoverAnim()
{
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ButtonHover);
	StopAnimation(UnHoverAnim);
	PlayAnimation(HoverAnim);
}

void UUpgradeDetailsPanelUI::PlayUnHoverAnim()
{
	StopAnimation(HoverAnim);
	PlayAnimation(UnHoverAnim);
}

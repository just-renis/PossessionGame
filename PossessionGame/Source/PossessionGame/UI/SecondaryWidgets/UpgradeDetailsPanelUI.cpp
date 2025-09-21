#include "UpgradeDetailsPanelUI.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/UI/MainWidgets/UpgradeSelectionUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
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
	SetUpgradeChoiceBackground();
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

FString UUpgradeDetailsPanelUI::GetUpgradeTextFromSingularAttributeValue(const TPair<EUpgradeAttribute, float> InAttribute, const bool bForEnemy)
{
	if (InAttribute.Key == EUpgradeAttribute::None) return FString();
	const FString AttributeValue = ConvertUpgradeAttributeValueToString(InAttribute.Value);
	switch (InAttribute.Key)
	{
		case EUpgradeAttribute::FlatDamage: return GetRichTextTag(bForEnemy) + "+" + AttributeValue + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "Damage", "DAMAGE").ToString() + "</>\n";
		case EUpgradeAttribute::FlatHealth: return GetRichTextTag(bForEnemy) + "+" + AttributeValue + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "FlatHealth", "HEALTH").ToString() + "</>\n";
		case EUpgradeAttribute::DamagePercentage: return GetRichTextTag(bForEnemy) + "+" + AttributeValue + "%</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "Damage", "DAMAGE").ToString() + "</>\n";
		case EUpgradeAttribute::ProjectilesAmount: return GetRichTextTag(bForEnemy) + "+" + AttributeValue + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "Projectiles", "PROJECTILES").ToString() + "</>\n";
		case EUpgradeAttribute::AbilityCooldownReduction: return GetRichTextTag(bForEnemy) + "-" + AttributeValue + "%</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "AbilityCooldown", "ABILITY COOLDOWN").ToString() + "</>\n";
		case EUpgradeAttribute::AbilityChargesAmount: return GetRichTextTag(bForEnemy) + "+" + AttributeValue + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "AbilityCharges", "ABILITY CHARGES").ToString() + "</>\n";
		case EUpgradeAttribute::Size: return GetRichTextTag(bForEnemy) + "+" + AttributeValue + "%</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "Area", "AREA").ToString() + "</>\n";
		case EUpgradeAttribute::AbilityDuration: return GetRichTextTag(bForEnemy) + "+" + AttributeValue + "%</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "AbilityDuration", "ABILITY DURATION").ToString() + "</>\n";
		case EUpgradeAttribute::EffectDuration: return GetRichTextTag(bForEnemy) + "+" + AttributeValue + "%</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "EffectDuration", "EFFECT DURATION").ToString() + "</>\n";
		case EUpgradeAttribute::ReduceDamageTaken: return GetRichTextTag(bForEnemy) + "-" + AttributeValue + "%</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "DamageTaken", "DAMAGE TAKEN").ToString() + "</>\n";
		case EUpgradeAttribute::CloneAmount: return GetRichTextTag(bForEnemy) + "+" + AttributeValue + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "CloneAmount", "CLONE AMOUNT").ToString() + "</>\n";
		case EUpgradeAttribute::Cap: return GetRichTextTag(bForEnemy) + "+" + AttributeValue + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "Cap", "CAP").ToString() + "</>\n";
		case EUpgradeAttribute::ProjectileSpeed: return GetRichTextTag(bForEnemy) + "+" + AttributeValue + "%</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "ProjectileSpeed", "PROJECTILE SPEED").ToString() + "</>\n";
		default: return GetSpecialUpgradeText(InAttribute.Key, bForEnemy);
	}
}

FString UUpgradeDetailsPanelUI::GetRichTextTag(const bool bForEnemy) const
{
	if (bForEnemy) return FString("<UpgradeAttributeNumberRed>");
	switch (UpgradeChoice.Rarity)
	{
		case EUpgradeChoice::Common: return FString("<UpgradeAttributeNumberBlue>");
		case EUpgradeChoice::Rare: return FString("<UpgradeAttributeNumberGreen>");
		case EUpgradeChoice::Legendary: return FString("<UpgradeAttributeNumberYellow>");
	}
	return {};
}

FString UUpgradeDetailsPanelUI::GetSpecialUpgradeText(const EUpgradeAttribute InUpgradeAttribute, const bool bForEnemy)
{
	SpecialAttribute = InUpgradeAttribute;
	switch (UpgradeName)
	{
		case EUpgrade::GuardianAbility1:
			switch (InUpgradeAttribute)
			{
				case EUpgradeAttribute::Special1:
					if (!bForEnemy)
					{
						return "<UpgradeAttributeText>" + NSLOCTEXT("ST_Localization", "GuardianAbilitySpecial1.1", "WHILE ACTIVE, YOU REFLECT</>").ToString() +
						GetRichTextTag(false) + " 50% </>" + NSLOCTEXT("ST_Localization", "GuardianAbilitySpecial1.2", "<UpgradeAttributeText>OF THE DAMAGE.</>").ToString();
					}
					return NSLOCTEXT("ST_Localization", "GuardianAbilitySpecial1.3", "<UpgradeAttributeText>GUARDIAN TAKES NO DAMAGE ON FIRST HIT.</>").ToString();
				default: break;
			}
		case EUpgrade::GuardianAbility2:
			break;
		case EUpgrade::GuardianAbility3:
			switch (InUpgradeAttribute)
			{
				case EUpgradeAttribute::Special1:
					if (!bForEnemy)
					{
						return "<UpgradeAttributeText>" + NSLOCTEXT("ST_Localization", "GuardianAbilitySpecial3.1", "EACH ENEMY HIT PASSES DAMAGE TO THE CLOSEST ENEMY.</>").ToString();
					}
					return NSLOCTEXT("ST_Localization", "GuardianAbilitySpecial3.2", "<UpgradeAttributeText>PROJECTILES BOUNCE</>").ToString() +
					GetRichTextTag(true) + " 2 </>" + NSLOCTEXT("ST_Localization", "GuardianAbilitySpecial3.3", "<UpgradeAttributeText>TIMES.</>").ToString();
				default: break;
			}
			break;
		case EUpgrade::RangerAbility1:
			switch (InUpgradeAttribute)
			{
				case EUpgradeAttribute::Special1:
					if (!bForEnemy)
					{
						return "<UpgradeAttributeText>" + NSLOCTEXT("ST_Localization", "RangerAbilitySpecial1.1", "AFTER USING GRENADE LAUNCHER</>").ToString() +
						GetRichTextTag(false) + " +10% </>" + NSLOCTEXT("ST_Localization", "RangerAbilitySpecial1.2", "<UpgradeAttributeText>DAMAGE FOR</>").ToString() +
						GetRichTextTag(false) + " 15 </>" + NSLOCTEXT("ST_Localization", "RangerAbilitySpecial1.3", "<UpgradeAttributeText>SECONDS.</>").ToString();
					}
					return NSLOCTEXT("ST_Localization", "RangerAbilitySpecial1.4", "<UpgradeAttributeText>EACH RANGER UPGRADE GIVES</>").ToString() +
					GetRichTextTag(true) + " +10% </>" + NSLOCTEXT("ST_Localization", "RangerAbilitySpecial1.5", "<UpgradeAttributeText>DAMAGE</>").ToString() +
					GetRichTextTag(true) + " -10% </>" + NSLOCTEXT("ST_Localization", "RangerAbilitySpecial1.6", "<UpgradeAttributeText>COOLDOWN.</>").ToString();
				default: break;
			}
			break;
		case EUpgrade::RangerAbility2:
			break;
		case EUpgrade::RangerAbility3:
			switch (InUpgradeAttribute)
			{
				case EUpgradeAttribute::Special1:
					if (!bForEnemy)
					{
						return "<UpgradeAttributeText>" + NSLOCTEXT("ST_Localization", "RangerAbilitySpecial3.1", "MINE DAMAGE INCREASES NEAR IT'S CENTER UP TO</>").ToString() +
						GetRichTextTag(false) + " 2X.</>";
					}
					return NSLOCTEXT("ST_Localization", "RangerAbilitySpecial3.2", "<UpgradeAttributeText>THE CLOSER DAMAGE IS TO RANGER, THE LESS DAMAGE RANGER TAKES.</>").ToString();
				default: break;
			}
			break;
		case EUpgrade::TricksterAbility1:
			switch (InUpgradeAttribute)
			{
				case EUpgradeAttribute::Special1:
					if (!bForEnemy)
					{
						return "<UpgradeAttributeText>" + NSLOCTEXT("ST_Localization", "TricksterAbilitySpecial1.1", "ENEMIES IN POISON SMOKE ARE</>").ToString() +
						GetRichTextTag(false) + " 3x </>" + NSLOCTEXT("ST_Localization", "TricksterAbilitySpecial1.2", "<UpgradeAttributeText>SLOWED.</>").ToString();
					}
					return "";//GetRichTextTag(true) + "+30</> HEALTH\n" + GetRichTextTag(true) + "+40%</> DAMAGE"; // PLACEHOLDER FOR SLOWING 2x PLAYER IN ENEMY POISON SMOKE.
				default: break;
			}
			break;
		case EUpgrade::TricksterAbility2:
			break;
		case EUpgrade::TricksterAbility3:
			switch (InUpgradeAttribute)
			{
				case EUpgradeAttribute::Special1:
					if (!bForEnemy)
					{
						return "<UpgradeAttributeText>" + NSLOCTEXT("ST_Localization", "TricksterAbilitySpecial3.1", "PLAYER LEVEL BOOSTS APPLIES TO CLONES.</>").ToString();
					}
					return NSLOCTEXT("ST_Localization", "TricksterAbilitySpecial3.2", "<UpgradeAttributeText>RUNS TO PLAYER AT LIGHT SPEED.</>").ToString();
				default: break;
			}
			break;
		default: ;
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
	for (const TPair<EUpgradeAttribute, float> Attribute : UpgradeAttributesData.UpgradeAttributes) UpgradeTextString += GetUpgradeTextFromSingularAttributeValue(Attribute, !bIsPlayerUpgrade);
	UpgradeTextString.RemoveFromEnd(TEXT("\n"));
	if (bIsPlayerUpgrade) PlayerUpgradeText->SetText(FText::FromString(UpgradeTextString));
	else EnemyUpgradeText->SetText(FText::FromString(UpgradeTextString));
}

void UUpgradeDetailsPanelUI::SetUpgradeChoiceBackground() { UpgradeChoiceBackground->SetBrushFromAtlasInterface(Textures[static_cast<uint8>(UpgradeChoice.Rarity)]); }

void UUpgradeDetailsPanelUI::SelectUpgrade()
{
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::UpgradeCardSelect);
	bHasBeenSelected = true;
	PlayAnimation(PressAnim);
	GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->AddUpgrade(GetGameInstance(), UpgradeName, UpgradeChoice, SpecialAttribute);
	SpecialAttribute = EUpgradeAttribute::None;
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

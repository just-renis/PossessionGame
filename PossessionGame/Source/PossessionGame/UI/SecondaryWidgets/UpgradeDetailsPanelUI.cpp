#include "UpgradeDetailsPanelUI.h"
#include "Components/Button.h"
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

void UUpgradeDetailsPanelUI::SetButtonState(const bool bState) const
{
	if (!UpgradeChoiceButton) return;
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

FText UUpgradeDetailsPanelUI::GetUpgradeTextFromSingularAttributeValue(const TPair<EUpgradeAttribute, float> InAttribute, const bool bForEnemy)
{
	if (InAttribute.Key == EUpgradeAttribute::None) return FText();
	const FString AttributeValue = ConvertUpgradeAttributeValueToString(InAttribute.Value);
	switch (InAttribute.Key)
	{
		case EUpgradeAttribute::FlatDamage: return FText::FromString(GetRichTextTag(bForEnemy) + "+" + AttributeValue +"</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "Damage", "DAMAGE").ToString() + "</>");
		case EUpgradeAttribute::FlatHealth: return FText::FromString(GetRichTextTag(bForEnemy) + "+" + AttributeValue +"</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "FlatHealth", "HEALTH").ToString() + "</>");
		case EUpgradeAttribute::DamagePercentage: return FText::FromString(GetRichTextTag(bForEnemy) + "+" + AttributeValue +"%</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "Damage", "DAMAGE").ToString() + "</>");
		case EUpgradeAttribute::ProjectilesAmount: return FText::FromString(GetRichTextTag(bForEnemy) + "+" + AttributeValue +"</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "Projectiles", "PROJECTILES").ToString() + "</>");
		case EUpgradeAttribute::AbilityCooldownReduction: return FText::FromString(GetRichTextTag(bForEnemy) + "-" + AttributeValue +"%</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "AbilityCooldown", "ABILITY COOLDOWN").ToString() + "</>");
		case EUpgradeAttribute::AbilityChargesAmount: return FText::FromString(GetRichTextTag(bForEnemy) + "+" + AttributeValue +"</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "AbilityCharges", "ABILITY CHARGES").ToString() + "</>");
		case EUpgradeAttribute::Size: return FText::FromString(GetRichTextTag(bForEnemy) + "+" + AttributeValue +"%</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "Area", "AREA").ToString() + "</>");
		case EUpgradeAttribute::AbilityDuration: return FText::FromString(GetRichTextTag(bForEnemy) + "+" + AttributeValue +"%</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "AbilityDuration", "ABILITY DURATION").ToString() + "</>");
		case EUpgradeAttribute::EffectDuration: return FText::FromString(GetRichTextTag(bForEnemy) + "+" + AttributeValue +"%</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "EffectDuration", "EFFECT DURATION").ToString() + "</>");
		case EUpgradeAttribute::ReduceDamageTaken: return FText::FromString(GetRichTextTag(bForEnemy) + "-" + AttributeValue +"%</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "DamageTaken", "DAMAGE TAKEN").ToString() + "</>");
		case EUpgradeAttribute::CloneAmount: return FText::FromString(GetRichTextTag(bForEnemy) + "+" + AttributeValue +"</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "CloneAmount", "CLONE AMOUNT").ToString() + "</>");
		case EUpgradeAttribute::Cap: return FText::FromString(GetRichTextTag(bForEnemy) + "+" + AttributeValue +"</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "Cap", "CAP").ToString() + "</>");
		case EUpgradeAttribute::ProjectileSpeed: return FText::FromString(GetRichTextTag(bForEnemy) + "+" + AttributeValue +"%</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "ProjectileSpeed", "PROJECTILE SPEED").ToString() + "</>");
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

FText UUpgradeDetailsPanelUI::GetSpecialUpgradeText(const EUpgradeAttribute InUpgradeAttribute, const bool bForEnemy)
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
						return FText::Format(NSLOCTEXT("ST_Localization", "GuardianAbilitySpecial1.1",
							"<UpgradeAttributeText>WHILE ACTIVE, YOU REFLECT</>{0}{1}<UpgradeAttributeText>OF THE DAMAGE.</>"),
							FText::FromString(GetRichTextTag(false)), FText::FromString(" 50% </>"));
					}
					return NSLOCTEXT("ST_Localization", "GuardianAbilitySpecial1.2", "<UpgradeAttributeText>GUARDIAN TAKES NO DAMAGE ON FIRST HIT.</>");
				default: break;
			}
		case EUpgrade::GuardianAbility2:
			switch (InUpgradeAttribute)
			{
				case EUpgradeAttribute::Special1:
					if (!bForEnemy)
					{
						return FText::Format(NSLOCTEXT("ST_Localization", "GuardianAbilitySpecial2.1",
							"<UpgradeAttributeText>FOR EACH ENEMY KILLED WHILE ABILITY ACTIVE</>{0}{1}<UpgradeAttributeText>MORE DAMAGE FOR</>{2}{3}<UpgradeAttributeText>SECS.</>"),
							FText::FromString(GetRichTextTag(false)), FText::FromString(" 5% </>"), FText::FromString(GetRichTextTag(false)), FText::FromString(" 20 </>"));
					}
					return FText::Format(NSLOCTEXT("ST_Localization", "GuardianAbilitySpecial2.2",
						"<UpgradeAttributeText>WHEN ABILITY ENDS, ALL ENEMIES RECEIVE</>{0}{1}<UpgradeAttributeText>ACTION SPEED FOR</>{2}{3}<UpgradeAttributeText>SECS.</>"),
						FText::FromString(GetRichTextTag(true)), FText::FromString(" 50% </>"), FText::FromString(GetRichTextTag(true)), FText::FromString(" 5 </>"));
				default: break;
			}
			break;
		case EUpgrade::GuardianAbility3:
			switch (InUpgradeAttribute)
			{
				case EUpgradeAttribute::Special1:
					if (!bForEnemy) return NSLOCTEXT("ST_Localization", "GuardianAbilitySpecial3.1", "<UpgradeAttributeText>EACH ENEMY HIT PASSES DAMAGE TO THE CLOSEST ENEMY.</>");
					return FText::Format(NSLOCTEXT("ST_Localization", "GuardianAbilitySpecial3.2",
						"<UpgradeAttributeText>PROJECTILES BOUNCE</>{0}{1}<UpgradeAttributeText>TIMES.</>"),
						FText::FromString(GetRichTextTag(true)), FText::FromString(" 2 </>"));
				default: break;
			}
			break;
		case EUpgrade::RangerAbility1:
			switch (InUpgradeAttribute)
			{
				case EUpgradeAttribute::Special1:
					if (!bForEnemy)
					{
						return FText::Format(NSLOCTEXT("ST_Localization", "RangerAbilitySpecial1.1",
							"<UpgradeAttributeText>AFTER USING GRENADE LAUNCHER</>{0}{1}<UpgradeAttributeText>DAMAGE FOR</>{2}{3}<UpgradeAttributeText>SECS.</>"),
							FText::FromString(GetRichTextTag(false)), FText::FromString(" +10% </>"), FText::FromString(GetRichTextTag(false)), FText::FromString(" 15 </>"));
					}
					return FText::Format(NSLOCTEXT("ST_Localization", "RangerAbilitySpecial1.2",
						"<UpgradeAttributeText>EACH RANGER UPGRADE GIVES</>{0}{1}<UpgradeAttributeText>DAMAGE</>{2}{3}<UpgradeAttributeText>COOLDOWN.</>"),
						FText::FromString(GetRichTextTag(true)), FText::FromString(" +10% </>"), FText::FromString(GetRichTextTag(true)), FText::FromString(" -10% </>"));
				default: break;
			}
			break;
		case EUpgrade::RangerAbility2:
			switch (InUpgradeAttribute)
			{
				case EUpgradeAttribute::Special1:
					if (!bForEnemy)
					{
						return FText::Format(NSLOCTEXT("ST_Localization", "RangerAbilitySpecial2.1",
							"<UpgradeAttributeText>EACH HIT TO SHIELD RAISES PLAYER DAMAGE BY</>{0}{1}<UpgradeAttributeText>DAMAGE</>{2}{3}<UpgradeAttributeText>SECS.</>"),
							FText::FromString(GetRichTextTag(false)), FText::FromString(" 1% </>"), FText::FromString(GetRichTextTag(false)), FText::FromString(" 15 </>"));
					}
					return FText();
				default: break;
			}
			break;
		case EUpgrade::RangerAbility3:
			switch (InUpgradeAttribute)
			{
				case EUpgradeAttribute::Special1:
					if (!bForEnemy)
					{
						return FText::Format(NSLOCTEXT("ST_Localization", "RangerAbilitySpecial3.1",
							"<UpgradeAttributeText>MINE DAMAGE INCREASES NEAR IT'S CENTER UP TO</>{0}{1}<UpgradeAttributeText>.</>"),
							FText::FromString(GetRichTextTag(false)), FText::FromString(" 2X </>"));
					}
				return NSLOCTEXT("ST_Localization", "RangerAbilitySpecial3.2", "<UpgradeAttributeText>THE CLOSER DAMAGE IS TO RANGER, THE LESS DAMAGE RANGER TAKES.</>");
				default: break;
			}
			break;
		case EUpgrade::TricksterAbility1:
			switch (InUpgradeAttribute)
			{
				case EUpgradeAttribute::Special1:
					if (!bForEnemy)
					{
						return FText::Format(NSLOCTEXT("ST_Localization", "TricksterAbilitySpecial1.1",
							"<UpgradeAttributeText>ENEMIES IN POISON SMOKE ARE</>{0}{1}<UpgradeAttributeText>SLOWER.</>"),
							FText::FromString(GetRichTextTag(false)), FText::FromString(" 3X </>"));
					}
				return FText::Format(NSLOCTEXT("ST_Localization", "TricksterAbilitySpecial1.2",
					"<UpgradeAttributeText>PLAYER IN POISON SMOKE IS</>{0}{1}<UpgradeAttributeText>SLOWER.</>"),
					FText::FromString(GetRichTextTag(true)), FText::FromString(" 2X </>"));
				default: break;
			}
			break;
		case EUpgrade::TricksterAbility2:
			switch (InUpgradeAttribute)
			{
				case EUpgradeAttribute::Special1:
					if (!bForEnemy)
					{
						return FText::Format(NSLOCTEXT("ST_Localization", "TricksterAbilitySpecial2.1",
							"<UpgradeAttributeText>KILLED ENEMY AFFECTED BY EMP EXPLODES AND DEALS</>{0}{1}<UpgradeAttributeText>OF THEIR MAX HP DAMAGE TO OTHER ENEMIES.</>"),
							FText::FromString(GetRichTextTag(false)), FText::FromString(" 25% </>"));
					}
				return NSLOCTEXT("ST_Localization", "TricksterAbilitySpecial2.2", "<UpgradeAttributeText>TRICKSTER IS IMMUNE TO EMP.</>");
				default: break;
			}
			break;
		case EUpgrade::TricksterAbility3:
			switch (InUpgradeAttribute)
			{
				case EUpgradeAttribute::Special1:
					if (!bForEnemy) return NSLOCTEXT("ST_Localization", "TricksterAbilitySpecial3.1", "<UpgradeAttributeText>PLAYER LEVEL BOOSTS APPLIES TO CLONES.</>");
				return NSLOCTEXT("ST_Localization", "TricksterAbilitySpecial3.2", "<UpgradeAttributeText>RUNS TO PLAYER AT LIGHT SPEED.</>");
				default: break;
			}
			break;
		default: ;
	}
	return FText();
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
		if (bIsPlayerUpgrade) PlayerUpgradeText->SetText(NSLOCTEXT("ST_Localization", "NoAvailableChoice", "No available choice"));
		else EnemyUpgradeText->SetText(NSLOCTEXT("ST_Localization", "NoAvailableChoice", "No available choice"));
		return;
	}
	FUpgradeAttributes UpgradeAttributesData = bIsPlayerUpgrade ? UpgradeChoice.PlayerAttributes : UpgradeChoice.EnemyAttributes;
	TArray<FText> Parts;
	for (const TPair<EUpgradeAttribute, float> Attribute : UpgradeAttributesData.UpgradeAttributes) Parts.Add(GetUpgradeTextFromSingularAttributeValue(Attribute, !bIsPlayerUpgrade));
	if (bIsPlayerUpgrade) PlayerUpgradeText->SetText(FText::Join(FText::FromString(TEXT("\n")), Parts));
	else EnemyUpgradeText->SetText(FText::Join(FText::FromString(TEXT("\n")), Parts));
}

void UUpgradeDetailsPanelUI::SetUpgradeChoiceBackground()
{
	FButtonStyle Style = UpgradeChoiceButton->GetStyle();
	const uint8 Index = static_cast<uint8>(UpgradeChoice.Rarity);
	Style.Normal.SetResourceObject(NormalTextures[Index]);
	Style.Hovered.SetResourceObject(HoverTextures[Index]);
	Style.Pressed.SetResourceObject(HoverTextures[Index]);
	UpgradeChoiceButton->SetStyle(Style);
}

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

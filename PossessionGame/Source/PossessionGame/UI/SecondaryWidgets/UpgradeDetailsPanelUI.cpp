#include "UpgradeDetailsPanelUI.h"
#include "Components/Button.h"
#include "Components/RichTextBlock.h"
#include "PossessionGame/Data Assets/Base Data Assets/AbilityData.h"
#include "PossessionGame/Data Assets/Base Data Assets/CharacterData.h"
#include "PossessionGame/Data Assets/Base Data Assets/CodexData.h"
#include "PossessionGame/Data Assets/Base Data Assets/CodexProjectileData.h"
#include "PossessionGame/Data Assets/Base Data Assets/ThrowableData.h"
#include "PossessionGame/Enums/EAbility.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Enums/EUpgradeType.h"
#include "PossessionGame/UI/MainWidgets/UpgradeSelectionUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/GlobalSettingsManager.h"
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

ECharacterType UUpgradeDetailsPanelUI::GetCharacterByUpgradeName() const
{
	switch (UpgradeName)
	{
		case EUpgrade::RangerAbility1: case EUpgrade::RangerAbility2: case EUpgrade::RangerAbility3: return ECharacterType::Ranger;
		case EUpgrade::TricksterAbility1: case EUpgrade::TricksterAbility2: case EUpgrade::TricksterAbility3: return ECharacterType::Trickster;
		case EUpgrade::GuardianAbility1: case EUpgrade::GuardianAbility2: case EUpgrade::GuardianAbility3: return ECharacterType::Guardian;
		case EUpgrade::BreacherAbility1: case EUpgrade::BreacherAbility2: case EUpgrade::BreacherAbility3: return ECharacterType::Breacher;
		case EUpgrade::TankAbility1: case EUpgrade::TankAbility2: return ECharacterType::Tank;
		default: return ECharacterType::Random;
	}
}

EAbility UUpgradeDetailsPanelUI::GetAbilityByUpgradeName() const
{
	switch (UpgradeName)
	{
		case EUpgrade::RangerAbility1: return EAbility::RangerAbility1;
		case EUpgrade::RangerAbility2: return EAbility::RangerAbility2;
		case EUpgrade::RangerAbility3: return EAbility::RangerAbility3;
		case EUpgrade::TricksterAbility1: return EAbility::TricksterAbility1;
		case EUpgrade::TricksterAbility2: return EAbility::TricksterAbility2;
		case EUpgrade::TricksterAbility3: return EAbility::TricksterAbility3;
		case EUpgrade::GuardianAbility1: return EAbility::GuardianAbility1;
		case EUpgrade::GuardianAbility2: return EAbility::GuardianAbility2;
		case EUpgrade::GuardianAbility3: return EAbility::GuardianAbility3;
		case EUpgrade::BreacherAbility1: return EAbility::BreacherAbility1;
		case EUpgrade::BreacherAbility2: return EAbility::BreacherAbility2;
		case EUpgrade::BreacherAbility3: return EAbility::BreacherAbility3;
		case EUpgrade::TankAbility1: return EAbility::TankAbility1;
		case EUpgrade::TankAbility2: return EAbility::TankAbility2;
		default: return EAbility::None;
	}
}

FString UUpgradeDetailsPanelUI::SanitizeFloatValueToString(const float InValue)
{
	FString FloatValueString = FString::Printf(TEXT("%.1f"), InValue);
	if (FloatValueString.EndsWith(TEXT(".0"))) FloatValueString = FloatValueString.LeftChop(2);
	return FloatValueString;
}

FText UUpgradeDetailsPanelUI::GetUpgradeTextFromSingularAttributeValue(const TPair<EUpgradeAttribute, float> InAttribute, const bool bForEnemy)
{
	if (InAttribute.Key == EUpgradeAttribute::None) return FText();
	const TObjectPtr<URunSaveSlot> RunSaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot();
	const FString AttributeValue = SanitizeFloatValueToString(InAttribute.Value);
	const ECharacterType CharacterName = GetCharacterByUpgradeName();
	const TObjectPtr<UCodexData> CodexData = GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetCodexData();
	const int32 FlatHealth = CodexData->GetCharacterData(false, CharacterName)->GetMaxHp() + RunSaveSlot->GetHpUpgradesForEnemy(CharacterName);
	FString AbilityCooldownSecsString = "";
	FString AbilityDurationString = "";
	FString AbilityChargesAmountString = "";
	FString DamageIncreaseString = "+" + AttributeValue + "%";
	FString FlatDamageString = "+" + AttributeValue;
	FString ProjectilesAmountString = "";
	FString SizeString = "";
	FString EffectDurationString = "";
	FString ReduceDamageTakenString = "";
	FString CloneAmountString = "";
	FString CapString = "";
	FString ProjectileSpeedString = "";
	if (const TObjectPtr<UAbilityData> AbilityData = CodexData->GetAbilityData(!bForEnemy, GetAbilityByUpgradeName()))
	{
		if (InAttribute.Key == EUpgradeAttribute::AbilityCooldownReduction)
		{
			const float CurrentValueFromUpgrades = RunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::AbilityCooldownReduction, !bForEnemy);
			AbilityCooldownSecsString = GetUpgradeChangeString(CurrentValueFromUpgrades, AbilityData->GetCooldownSecs(), InAttribute.Value, EUpgradeType::Reduction, true);
		}
		if (InAttribute.Key == EUpgradeAttribute::AbilityDuration)
		{
			const float CurrentValueFromUpgrades = RunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::AbilityDuration, !bForEnemy);
			AbilityDurationString = GetUpgradeChangeString(CurrentValueFromUpgrades, AbilityData->GetAbilityDuration(), InAttribute.Value, EUpgradeType::Increase, true);
		}
		if (InAttribute.Key == EUpgradeAttribute::AbilityChargesAmount)
		{
			const float CurrentValueFromUpgrades = RunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::AbilityChargesAmount, !bForEnemy);
			AbilityChargesAmountString = GetUpgradeChangeString(CurrentValueFromUpgrades, AbilityData->GetChargesAmount(), InAttribute.Value, EUpgradeType::Addition, false);
		}
	}
	if (!bForEnemy)
	{
		if (UpgradeName == EUpgrade::RangerAbility3 && InAttribute.Key == EUpgradeAttribute::DamagePercentage)
		{
			const TObjectPtr<UThrowableData> MineData = CodexData->GetThrowableData(!bForEnemy, EPoolObject::RangerMine);
			
			const float CurrentAmountFromUpgrades = RunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::FlatDamage, !bForEnemy);
			const float CurrentPercentageFromUpgrades = RunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::DamagePercentage, !bForEnemy);
		
			const float CurrentAmount = MineData->GetDamage() + CurrentAmountFromUpgrades;
			const float NextAmount = MineData->GetDamage() + InAttribute.Value + CurrentAmountFromUpgrades;
			const float CurrentPercentage = MineData->GetDamage() * (1.f + CurrentPercentageFromUpgrades * 0.01f);
			const float NextPercentage = MineData->GetDamage() * (1.f + (InAttribute.Value + CurrentPercentageFromUpgrades) * 0.01f);
			FlatDamageString = SanitizeFloatValueToString(CurrentAmount) + "->" + SanitizeFloatValueToString(NextAmount);
			DamageIncreaseString = SanitizeFloatValueToString(CurrentPercentage) + "->" + SanitizeFloatValueToString(NextPercentage);
		}
		else if (UpgradeName == EUpgrade::RangerAbility1)
		{
			const TObjectPtr<UCodexProjectileData> ProjectileData = CodexData->GetProjectileData(!bForEnemy, EPoolObject::RangerGrenadeLauncherProjectile);

			const float CurrentAmountFromUpgrades = RunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::FlatDamage, !bForEnemy);
			const float CurrentPercentageFromUpgrades = RunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::DamagePercentage, !bForEnemy);
		
			const float CurrentAmount = ProjectileData->GetDamage() + CurrentAmountFromUpgrades;
			const float NextAmount = ProjectileData->GetDamage() + InAttribute.Value + CurrentAmountFromUpgrades;
			const float CurrentPercentage = ProjectileData->GetDamage() * (1.f + CurrentPercentageFromUpgrades * 0.01f);
			const float NextPercentage = ProjectileData->GetDamage() * (1.f + (InAttribute.Value + CurrentPercentageFromUpgrades) * 0.01f);
		
			FlatDamageString = SanitizeFloatValueToString(CurrentAmount) + "->" + SanitizeFloatValueToString(NextAmount);
			DamageIncreaseString = SanitizeFloatValueToString(CurrentPercentage) + "->" + SanitizeFloatValueToString(NextPercentage);
		}
	}
	switch (InAttribute.Key)
	{
		// DONE
		case EUpgradeAttribute::FlatHealth: return FText::FromString(GetRichTextTag(bForEnemy) + FString::FormatAsNumber(FlatHealth) + "->" + FString::FormatAsNumber(FlatHealth + static_cast<int32>(InAttribute.Value)) + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "FlatHealth", "HEALTH").ToString() + "</>");
		case EUpgradeAttribute::AbilityCooldownReduction: return FText::FromString(GetRichTextTag(bForEnemy) + AbilityCooldownSecsString + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "AbilityCooldown", "ABILITY COOLDOWN").ToString() + "</>");
		case EUpgradeAttribute::AbilityChargesAmount: return FText::FromString(GetRichTextTag(bForEnemy) + AbilityChargesAmountString + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "AbilityCharges", "ABILITY CHARGES").ToString() + "</>");
		case EUpgradeAttribute::AbilityDuration: return FText::FromString(GetRichTextTag(bForEnemy) + AbilityDurationString + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "AbilityDuration", "ABILITY DURATION").ToString() + "</>");
		case EUpgradeAttribute::FlatDamage: return FText::FromString(GetRichTextTag(bForEnemy) + FlatDamageString + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "Damage", "DAMAGE").ToString() + "</>");
		case EUpgradeAttribute::DamagePercentage: return FText::FromString(GetRichTextTag(bForEnemy) + DamageIncreaseString + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "Damage", "DAMAGE").ToString() + "</>");

		case EUpgradeAttribute::ProjectilesAmount: return FText::FromString(GetRichTextTag(bForEnemy) + "+" + AttributeValue + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "Projectiles", "PROJECTILES").ToString() + "</>");

		case EUpgradeAttribute::Size: return FText::FromString(GetRichTextTag(bForEnemy) + "+" + AttributeValue + "%</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "Area", "AREA").ToString() + "</>");

		case EUpgradeAttribute::EffectDuration: return FText::FromString(GetRichTextTag(bForEnemy) + "+" + AttributeValue + "%</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "EffectDuration", "EFFECT DURATION").ToString() + "</>");

		case EUpgradeAttribute::ReduceDamageTaken: return FText::FromString(GetRichTextTag(bForEnemy) + "-" + AttributeValue + "%</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "DamageTaken", "DAMAGE TAKEN").ToString() + "</>");

		case EUpgradeAttribute::CloneAmount: return FText::FromString(GetRichTextTag(bForEnemy) + "+" + AttributeValue + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "CloneAmount", "CLONE AMOUNT").ToString() + "</>");

		case EUpgradeAttribute::Cap: return FText::FromString(GetRichTextTag(bForEnemy) + "+" + AttributeValue + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "Cap", "CAP").ToString() + "</>");

		case EUpgradeAttribute::ProjectileSpeed: return FText::FromString(GetRichTextTag(bForEnemy) + "+" + AttributeValue + "%</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "ProjectileSpeed", "PROJECTILE SPEED").ToString() + "</>");

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

FString UUpgradeDetailsPanelUI::GetUpgradeChangeString(const float InCurrentValueFromUpgrades, const float InBaseValue, const float InUpgradeValue, const EUpgradeType InUpgradeType, const bool bIsTimeUpgrade)
{
	float CurrentChange = 0.f;
	float NextChange = 0.f;
	switch (InUpgradeType)
	{
		case EUpgradeType::Addition:
			CurrentChange = InBaseValue + InCurrentValueFromUpgrades;
			NextChange = InBaseValue + InUpgradeValue + InCurrentValueFromUpgrades;
			break;
		case EUpgradeType::Increase:
			CurrentChange = InBaseValue * (1.f + InCurrentValueFromUpgrades * 0.01f);
			NextChange = InBaseValue * (1.f + (InUpgradeValue + InCurrentValueFromUpgrades) * 0.01f);
			break;
		case EUpgradeType::Reduction:
			CurrentChange = InBaseValue / (1.f + InCurrentValueFromUpgrades * 0.01f);
			NextChange = InBaseValue / (1.f + (InUpgradeValue + InCurrentValueFromUpgrades) * 0.01f);
			break;
	}
	return SanitizeFloatValueToString(CurrentChange) + (bIsTimeUpgrade ? "s->" : "->") + SanitizeFloatValueToString(NextChange) + (bIsTimeUpgrade ? "s" : "");
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

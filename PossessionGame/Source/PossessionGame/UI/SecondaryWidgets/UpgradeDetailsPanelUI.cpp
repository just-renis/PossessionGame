#include "UpgradeDetailsPanelUI.h"
#include "Components/Button.h"
#include "Components/RichTextBlock.h"
#include "PossessionGame/Data Assets/Base Data Assets/AbilityData.h"
#include "PossessionGame/Data Assets/Base Data Assets/CharacterData.h"
#include "PossessionGame/Data Assets/Base Data Assets/CodexData.h"
#include "PossessionGame/Data Assets/Base Data Assets/CodexProjectileData.h"
#include "PossessionGame/Data Assets/Base Data Assets/DestructibleData.h"
#include "PossessionGame/Data Assets/Base Data Assets/MiscData.h"
#include "PossessionGame/Data Assets/Base Data Assets/ThrowableData.h"
#include "PossessionGame/Enums/EAbility.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Enums/EUpgradeType.h"
#include "PossessionGame/Enums/EUpgradeUnit.h"
#include "PossessionGame/UI/MainWidgets/UpgradeSelectionUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
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

FString UUpgradeDetailsPanelUI::GetUpgradeUnitString(const EUpgradeUnit InUpgradeUnit)
{
	switch (InUpgradeUnit)
	{
		case EUpgradeUnit::None: return FString("");
		case EUpgradeUnit::Seconds: return FString("s");
		case EUpgradeUnit::MetresPerSecond: return FString("m/s");
		case EUpgradeUnit::Metres: return FString("m");
		case EUpgradeUnit::Scale: return FString("X");
		case EUpgradeUnit::Percentage: return FString("%");
	}
	return FString();
}

FText UUpgradeDetailsPanelUI::GetUpgradeTextFromSingularAttributeValue(const TPair<EUpgradeAttribute, float> InAttribute, const bool bForEnemy)
{
	if (InAttribute.Key == EUpgradeAttribute::None) return FText();
	const TObjectPtr<URunSaveSlot> RunSaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot();
	const TObjectPtr<UCodexData> CodexData = GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetCodexData();
	switch (InAttribute.Key)
	{
		case EUpgradeAttribute::FlatHealth: return FText::FromString(GetRichTextTag(bForEnemy) + GetHealthUpgradeString(CodexData, RunSaveSlot, !bForEnemy, InAttribute) + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "FlatHealth", "HEALTH").ToString() + "</>");
		case EUpgradeAttribute::AbilityCooldownReduction: return FText::FromString(GetRichTextTag(bForEnemy) + GetAbilityUpgradeString(CodexData, RunSaveSlot, !bForEnemy, InAttribute) + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "Cooldown", "COOLDOWN").ToString() + "</>");
		case EUpgradeAttribute::AbilityChargesAmount: return FText::FromString(GetRichTextTag(bForEnemy) + GetAbilityUpgradeString(CodexData, RunSaveSlot, !bForEnemy, InAttribute) + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "Charges", "CHARGES").ToString() + "</>");
		case EUpgradeAttribute::AbilityDuration: return FText::FromString(GetRichTextTag(bForEnemy) + GetAbilityUpgradeString(CodexData, RunSaveSlot, !bForEnemy, InAttribute) + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "Duration", "DURATION").ToString() + "</>");
		case EUpgradeAttribute::FlatDamage: return FText::FromString(GetRichTextTag(bForEnemy) + (bForEnemy ? "+" + SanitizeFloatValueToString(InAttribute.Value) : GetDamageUpgradeString(CodexData, RunSaveSlot, !bForEnemy, InAttribute)) + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "Damage", "DAMAGE").ToString() + "</>");
		case EUpgradeAttribute::DamagePercentage: return FText::FromString(GetRichTextTag(bForEnemy) + (bForEnemy ? "+" + SanitizeFloatValueToString(InAttribute.Value) + "%" : GetDamageUpgradeString(CodexData, RunSaveSlot, !bForEnemy, InAttribute)) + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "Damage", "DAMAGE").ToString() + "</>");
		case EUpgradeAttribute::ProjectilesAmount: return FText::FromString(GetRichTextTag(bForEnemy) + GetProjectileAmountUpgradeString(CodexData, RunSaveSlot, !bForEnemy, InAttribute) + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "Projectiles", "PROJECTILES").ToString() + "</>");
		case EUpgradeAttribute::ProjectileSpeed: return FText::FromString(GetRichTextTag(bForEnemy) + GetProjectileSpeedUpgradeString(CodexData, RunSaveSlot, !bForEnemy, InAttribute) + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "ProjectileSpeed", "PROJECTILE SPEED").ToString() + "</>");
		case EUpgradeAttribute::Cap: return FText::FromString(GetRichTextTag(bForEnemy) + GetCapUpgradeString(CodexData, RunSaveSlot, !bForEnemy, InAttribute) + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "Cap", "CAP").ToString() + "</>");
		case EUpgradeAttribute::CloneAmount: return FText::FromString(GetRichTextTag(bForEnemy) + GetCloneAmountUpgradeString(CodexData, RunSaveSlot, !bForEnemy, InAttribute) + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "CloneAmount", "CLONE AMOUNT").ToString() + "</>");
		case EUpgradeAttribute::Size: return FText::FromString(GetRichTextTag(bForEnemy) + GetSizeUpgradeString(CodexData, RunSaveSlot, !bForEnemy, InAttribute) + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "Area", "AREA").ToString() + "</>");
		case EUpgradeAttribute::EffectDuration: return FText::FromString(GetRichTextTag(bForEnemy) + GetEffectDurationUpgradeString(CodexData, RunSaveSlot, !bForEnemy, InAttribute) + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "EffectDuration", "EFFECT DURATION").ToString() + "</>");
		case EUpgradeAttribute::ReduceDamageTaken: return FText::FromString(GetRichTextTag(bForEnemy) + GetDamageReductionUpgradeString(CodexData, RunSaveSlot, !bForEnemy, InAttribute) + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "DamageTaken", "DAMAGE TAKEN").ToString() + "</>");
		case EUpgradeAttribute::Impulse: return FText::FromString(GetRichTextTag(bForEnemy) + GetImpulseUpgradeString(CodexData, RunSaveSlot, !bForEnemy, InAttribute) + "</><UpgradeAttributeText> " + NSLOCTEXT("ST_Localization", "Impulse", "IMPULSE").ToString() + "</>");
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

FString UUpgradeDetailsPanelUI::GetUpgradeChangeString(const float InCurrentValueFromUpgrades, const float InBaseValue, const float InUpgradeValue, const EUpgradeType InUpgradeType, const EUpgradeUnit InUpgradeUnit)
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
	switch (InUpgradeUnit)
	{
		case EUpgradeUnit::MetresPerSecond: case EUpgradeUnit::Metres:
			CurrentChange *= 0.01f;
			NextChange *= 0.01f;
			break;
		case EUpgradeUnit::Percentage:
			CurrentChange *= 100.f;
			NextChange *= 100.f;
			break;
		default: break;
	}
	return SanitizeFloatValueToString(CurrentChange) + GetUpgradeUnitString(InUpgradeUnit) + "->" + SanitizeFloatValueToString(NextChange) + GetUpgradeUnitString(InUpgradeUnit);
}

FString UUpgradeDetailsPanelUI::GetHealthUpgradeString(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute)
{
	if (!bForPlayer)
	{
		if (const TObjectPtr<UCharacterData> CharacterData = InCodexData->GetCharacterData(bForPlayer, GetCharacterByUpgradeName()))
		{
			return GetUpgradeChangeString(InRunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::FlatHealth, bForPlayer),
			CharacterData->GetMaxHp(), InAttribute.Value, EUpgradeType::Addition, EUpgradeUnit::None);
		}
	}
	else
	{
		if (const TObjectPtr<UDestructibleData> DestructibleData = InCodexData->GetDestructibleData(bForPlayer, EPoolObject::RangerShield))
		{
			return GetUpgradeChangeString(InRunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::FlatHealth, bForPlayer),
			DestructibleData->GetHp(), InAttribute.Value, EUpgradeType::Addition, EUpgradeUnit::None);
		}
	}
	return FString();
}

FString UUpgradeDetailsPanelUI::GetAbilityUpgradeString(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute)
{
	if (const TObjectPtr<UAbilityData> AbilityData = InCodexData->GetAbilityData(bForPlayer, GetAbilityByUpgradeName()))
	{
		switch (InAttribute.Key)
		{
		case EUpgradeAttribute::AbilityCooldownReduction:
				return GetUpgradeChangeString(InRunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::AbilityCooldownReduction, bForPlayer),
				AbilityData->GetCooldownSecs(), InAttribute.Value, EUpgradeType::Reduction, EUpgradeUnit::Seconds);
			case EUpgradeAttribute::AbilityDuration:
				return GetUpgradeChangeString(InRunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::AbilityDuration, bForPlayer),
				AbilityData->GetAbilityDuration(), InAttribute.Value, EUpgradeType::Increase, EUpgradeUnit::Seconds);
			case EUpgradeAttribute::AbilityChargesAmount:
				return GetUpgradeChangeString(InRunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::AbilityChargesAmount, bForPlayer),
				AbilityData->GetChargesAmount(), InAttribute.Value, EUpgradeType::Addition, EUpgradeUnit::None);
			default: return FString();
		}
	}
	return FString();
}

FString UUpgradeDetailsPanelUI::GetDamageUpgradeString(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute)
{
	switch (UpgradeName)
	{
		case EUpgrade::RangerAbility1: return GetProjectileDamageByName(InCodexData, InRunSaveSlot, bForPlayer, InAttribute, EPoolObject::RangerGrenadeLauncherProjectile);
		case EUpgrade::RangerAbility3: return GetThrowableDamageByName(InCodexData, InRunSaveSlot, bForPlayer, InAttribute, EPoolObject::RangerMine);
		case EUpgrade::TricksterAbility1: return GetProjectileDamageByName(InCodexData, InRunSaveSlot, bForPlayer, InAttribute, EPoolObject::TricksterPoisonProjectile);
		case EUpgrade::TricksterAbility3: return GetProjectileDamageByName(InCodexData, InRunSaveSlot, bForPlayer, InAttribute, EPoolObject::CloneProjectile);
		case EUpgrade::GuardianAbility3: return GetProjectileDamageByName(InCodexData, InRunSaveSlot, bForPlayer, InAttribute, EPoolObject::VectorBlastProjectile);
		case EUpgrade::BreacherAbility1: return GetThrowableDamageByName(InCodexData, InRunSaveSlot, bForPlayer, InAttribute, EPoolObject::BreacherBarrel);
		case EUpgrade::BreacherAbility2: return GetMiscDamageUpgradeString(InCodexData, InRunSaveSlot, bForPlayer, InAttribute);
		case EUpgrade::BreacherAbility3: return GetThrowableDamageByName(InCodexData, InRunSaveSlot, bForPlayer, InAttribute, EPoolObject::BreacherDynamite);
		default: return FString();
	}
}

FString UUpgradeDetailsPanelUI::GetProjectileAmountUpgradeString(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute)
{
	switch (UpgradeName)
	{
		case EUpgrade::RangerAbility1: return GetProjectileAmountByName(InCodexData, InRunSaveSlot, bForPlayer, InAttribute, EPoolObject::RangerGrenadeLauncherProjectile);
		case EUpgrade::TricksterAbility1: return GetProjectileAmountByName(InCodexData, InRunSaveSlot, bForPlayer, InAttribute, EPoolObject::TricksterPoisonProjectile);
		case EUpgrade::TricksterAbility2: return GetProjectileAmountByName(InCodexData, InRunSaveSlot, bForPlayer, InAttribute, EPoolObject::TricksterEmpProjectile);
		case EUpgrade::GuardianAbility3: return GetProjectileAmountByName(InCodexData, InRunSaveSlot, bForPlayer, InAttribute, EPoolObject::VectorBlastProjectile);
		default: return FString();
	}
}

FString UUpgradeDetailsPanelUI::GetProjectileSpeedUpgradeString(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute)
{
	switch (UpgradeName)
	{
		case EUpgrade::GuardianAbility3: return GetProjectileSpeedByName(InCodexData, InRunSaveSlot, bForPlayer, InAttribute, EPoolObject::VectorBlastProjectile);
		default: return FString();
	}
}

FString UUpgradeDetailsPanelUI::GetCloneAmountUpgradeString(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute)
{
	switch (UpgradeName)
	{
		case EUpgrade::TricksterAbility3: return GetUpgradeChangeString(InRunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::CloneAmount, bForPlayer),
		1, InAttribute.Value, EUpgradeType::Addition, EUpgradeUnit::None);
		default: return FString();
	}
}

FString UUpgradeDetailsPanelUI::GetCapUpgradeString(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute)
{
	switch (UpgradeName)
	{
		case EUpgrade::RangerAbility3: return GetCapUpgradeByName(InCodexData, InRunSaveSlot, bForPlayer, InAttribute, EPoolObject::RangerMine);
		default: return FString();
	}
}

FString UUpgradeDetailsPanelUI::GetSizeUpgradeString(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute)
{
	switch (UpgradeName)
	{
		case EUpgrade::RangerAbility2: return GetSizeUpgradeByName(InCodexData, InRunSaveSlot, bForPlayer, InAttribute, EPoolObject::RangerShield);
		case EUpgrade::RangerAbility3: return GetSizeUpgradeByName(InCodexData, InRunSaveSlot, bForPlayer, InAttribute, EPoolObject::RangerMine);
		case EUpgrade::TricksterAbility1: return GetSizeUpgradeByName(InCodexData, InRunSaveSlot, bForPlayer, InAttribute, EPoolObject::TricksterPoisonProjectile);
		case EUpgrade::BreacherAbility1: return GetSizeUpgradeByName(InCodexData, InRunSaveSlot, bForPlayer, InAttribute, EPoolObject::BreacherBarrel);
		case EUpgrade::BreacherAbility3: return GetSizeUpgradeByName(InCodexData, InRunSaveSlot, bForPlayer, InAttribute, EPoolObject::BreacherDynamite);
		default: return FString();
	}
}

FString UUpgradeDetailsPanelUI::GetEffectDurationUpgradeString(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute)
{
	switch (UpgradeName)
	{
		case EUpgrade::RangerAbility2: return GetEffectUpgradeByName(InCodexData, InRunSaveSlot, bForPlayer, InAttribute, EPoolObject::RangerShield);
		case EUpgrade::TricksterAbility1: return GetEffectUpgradeByName(InCodexData, InRunSaveSlot, bForPlayer, InAttribute, EPoolObject::TricksterPoisonProjectile);
		case EUpgrade::TricksterAbility2: return GetEffectUpgradeByName(InCodexData, InRunSaveSlot, bForPlayer, InAttribute, EPoolObject::TricksterEmpProjectile);
		case EUpgrade::GuardianAbility3: return GetEffectUpgradeByName(InCodexData, InRunSaveSlot, bForPlayer, InAttribute, EPoolObject::VectorBlastProjectile);
		default: return FString();
	}
}

FString UUpgradeDetailsPanelUI::GetDamageReductionUpgradeString(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute)
{
	switch (UpgradeName)
	{
		case EUpgrade::GuardianAbility1:
			if (const TObjectPtr<UAbilityData> AbilityData = InCodexData->GetAbilityData(bForPlayer, GetAbilityByUpgradeName()))
			{
				return GetUpgradeChangeString(InRunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::ReduceDamageTaken, bForPlayer),
				AbilityData->GetBaseDamageReduction(), InAttribute.Value, EUpgradeType::Reduction, EUpgradeUnit::Percentage);
			}
			break;
		case EUpgrade::RangerAbility2:
			if (const TObjectPtr<UDestructibleData> DestructibleData = InCodexData->GetDestructibleData(bForPlayer, EPoolObject::RangerShield))
			{
				return GetUpgradeChangeString(InRunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::ReduceDamageTaken, bForPlayer),
				DestructibleData->GetDamageReduction(), InAttribute.Value, EUpgradeType::Reduction, EUpgradeUnit::Percentage);
			}
			break;
		default: return FString();
	}
	return FString();
}

FString UUpgradeDetailsPanelUI::GetImpulseUpgradeString(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute)
{
	if (const TObjectPtr<UAbilityData> AbilityData = InCodexData->GetAbilityData(bForPlayer, GetAbilityByUpgradeName()))
	{
		return GetUpgradeChangeString(InRunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::Impulse, bForPlayer),
		AbilityData->GetImpulseStrength(), InAttribute.Value, EUpgradeType::Increase, EUpgradeUnit::None);
	}
	return FString();
}

FString UUpgradeDetailsPanelUI::GetMiscDamageUpgradeString(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute)
{
	if (const TObjectPtr<UMiscData> MiscData = InCodexData->GetMiscData(bForPlayer, UpgradeName))
	{
		switch (InAttribute.Key)
		{
			case EUpgradeAttribute::FlatDamage: return GetUpgradeChangeString(InRunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::FlatDamage, bForPlayer),
			MiscData->GetDamage() + MiscData->GetDamage() * GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetLevelDamageIncrease() * 0.01f, InAttribute.Value, EUpgradeType::Addition, EUpgradeUnit::None);
			case EUpgradeAttribute::DamagePercentage: return GetUpgradeChangeString(InRunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::DamagePercentage, bForPlayer),
			MiscData->GetDamage(), InAttribute.Value, EUpgradeType::Increase, EUpgradeUnit::None);
			default: return FString();
		}
	}
	return FString();
}

FString UUpgradeDetailsPanelUI::GetProjectileAmountByName(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute, const EPoolObject InProjectileName)
{
	if (const TObjectPtr<UCodexProjectileData> ProjectileData = InCodexData->GetProjectileData(bForPlayer, InProjectileName))
	{
		return GetUpgradeChangeString(InRunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::ProjectilesAmount, bForPlayer),
		ProjectileData->GetAmount(), InAttribute.Value, EUpgradeType::Addition, EUpgradeUnit::None);
	}
	return FString();
}

FString UUpgradeDetailsPanelUI::GetProjectileSpeedByName(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute, const EPoolObject InProjectileName)
{
	if (const TObjectPtr<UCodexProjectileData> ProjectileData = InCodexData->GetProjectileData(bForPlayer, InProjectileName))
	{
		return GetUpgradeChangeString(InRunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::ProjectileSpeed, bForPlayer),
		ProjectileData->GetSpeed(), InAttribute.Value, EUpgradeType::Increase, EUpgradeUnit::MetresPerSecond);
	}
	return FString();
}

FString UUpgradeDetailsPanelUI::GetProjectileDamageByName(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute, const EPoolObject InProjectileName)
{
	if (const TObjectPtr<UCodexProjectileData> ProjectileData = InCodexData->GetProjectileData(bForPlayer, InProjectileName))
	{
		switch (InAttribute.Key)
		{
			case EUpgradeAttribute::DamagePercentage:
				return GetUpgradeChangeString(InRunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::DamagePercentage, bForPlayer),
				ProjectileData->GetDamage(), InAttribute.Value, EUpgradeType::Increase, EUpgradeUnit::None);
			case EUpgradeAttribute::FlatDamage:
				return GetUpgradeChangeString(InRunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::FlatDamage, bForPlayer),
				ProjectileData->GetDamage() + ProjectileData->GetDamage() * GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetLevelDamageIncrease() * 0.01f, InAttribute.Value, EUpgradeType::Addition, EUpgradeUnit::None);
			default: return FString();
		}
	}
	return FString();
}

FString UUpgradeDetailsPanelUI::GetThrowableDamageByName(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute, const EPoolObject InThrowableName)
{
	if (const TObjectPtr<UThrowableData> ThrowableData = InCodexData->GetThrowableData(bForPlayer, InThrowableName))
	{
		switch (InAttribute.Key)
		{
			case EUpgradeAttribute::DamagePercentage:
				return GetUpgradeChangeString(InRunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::DamagePercentage, bForPlayer),
				ThrowableData->GetDamage(), InAttribute.Value, EUpgradeType::Increase, EUpgradeUnit::None);
			case EUpgradeAttribute::FlatDamage:
				return GetUpgradeChangeString(InRunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::FlatDamage, bForPlayer),
				ThrowableData->GetDamage() + ThrowableData->GetDamage() * GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetLevelDamageIncrease() * 0.01f, InAttribute.Value, EUpgradeType::Addition, EUpgradeUnit::None);
			default: return FString();
		}
	}
	return FString();
}

FString UUpgradeDetailsPanelUI::GetCapUpgradeByName(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute, const EPoolObject InObjectName)
{
	if (const TObjectPtr<UThrowableData> MineData = InCodexData->GetThrowableData(bForPlayer, InObjectName))
	{
		return GetUpgradeChangeString(InRunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::Cap, bForPlayer),
		MineData->GetCap(), InAttribute.Value, EUpgradeType::Addition, EUpgradeUnit::None);
	}
	return FString();
}

FString UUpgradeDetailsPanelUI::GetSizeUpgradeByName(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute, const EPoolObject InObjectName)
{
	switch (InObjectName)
	{
		case EPoolObject::RangerShield:
			return GetUpgradeChangeString(InRunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::Size, bForPlayer),
			1.f, InAttribute.Value, EUpgradeType::Increase, EUpgradeUnit::Scale);
		case EPoolObject::RangerMine: case EPoolObject::BreacherBarrel: case EPoolObject::BreacherDynamite:
			if (const TObjectPtr<UThrowableData> ThrowableData = InCodexData->GetThrowableData(bForPlayer, InObjectName))
			{
				return GetUpgradeChangeString(InRunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::Size, bForPlayer),
				ThrowableData->GetDamageOuterRadius(), InAttribute.Value, EUpgradeType::Increase, EUpgradeUnit::Metres);
			}
			break;
		case EPoolObject::TricksterPoisonProjectile:
			if (const TObjectPtr<UCodexProjectileData> ProjectileData = InCodexData->GetProjectileData(bForPlayer, InObjectName))
			{
				return GetUpgradeChangeString(InRunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::Size, bForPlayer),
				ProjectileData->GetDamageOuterRadius(), InAttribute.Value, EUpgradeType::Increase, EUpgradeUnit::Metres);
			}
			break;
		default: return FString();
	}
	return FString();
}

FString UUpgradeDetailsPanelUI::GetEffectUpgradeByName(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute, const EPoolObject InObjectName)
{
	switch (InObjectName)
	{
		case EPoolObject::RangerShield:
			if (const TObjectPtr<UDestructibleData> DestructibleData = InCodexData->GetDestructibleData(bForPlayer, InObjectName))
			{
				return GetUpgradeChangeString(InRunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::EffectDuration, bForPlayer),
				DestructibleData->GetLifeSpan(), InAttribute.Value, EUpgradeType::Increase, EUpgradeUnit::Seconds);
			}
			break;
		case EPoolObject::TricksterPoisonProjectile:
		case EPoolObject::TricksterEmpProjectile:
		case EPoolObject::VectorBlastProjectile:
			if (const TObjectPtr<UCodexProjectileData> ProjectileData = InCodexData->GetProjectileData(bForPlayer, InObjectName))
			{
				return 	GetUpgradeChangeString(InRunSaveSlot->GetUpgradeAttributeValues(UpgradeName, EUpgradeAttribute::EffectDuration, bForPlayer),
				ProjectileData->GetEffectDuration(), InAttribute.Value, EUpgradeType::Increase, EUpgradeUnit::Seconds);
			}
			break;
		default: return FString();
	}
	return FString();
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

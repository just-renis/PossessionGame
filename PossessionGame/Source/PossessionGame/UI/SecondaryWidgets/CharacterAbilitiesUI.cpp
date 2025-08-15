#include "CharacterAbilitiesUI.h"
#include "AbilityIconUI.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "PossessionGame/Utilities/Data Utilities/FCharacterAbilities.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

TObjectPtr<UTexture2D> UCharacterAbilitiesUI::GetAbilityIcon(const int32 InAbilityIndex)
{
	for (int32 i = 0; i < AbilitiesData.Num(); i++)
	{
		if (AbilitiesData[i].AbilityIndex == InAbilityIndex) return AbilitiesData[i].AbilityIcon;
	}
	return nullptr;
}

void UCharacterAbilitiesUI::ResetFirstAbilitySlot()
{
	if (!FirstAbilitySlot) return;
	if (FirstAbilitySlot == FirstAbility) PlayAnimationForward(Select1To1ReverseAnim);
	else if (FirstAbilitySlot == SecondAbility) PlayAnimationForward(Select2To1ReverseAnim);
	FirstAbilitySlot->SetIsInSlot(false);
	FirstAbilitySlot = nullptr;
}

void UCharacterAbilitiesUI::ResetSecondAbilitySlot()
{
	if (!SecondAbilitySlot) return;
	if (SecondAbilitySlot == FirstAbility) PlayAnimationForward(Select1To2ReverseAnim);
	else if (SecondAbilitySlot == SecondAbility) PlayAnimationForward(Select2To2ReverseAnim);
	SecondAbilitySlot->SetIsInSlot(false);
	SecondAbilitySlot = nullptr;
}

void UCharacterAbilitiesUI::LoadAbilitiesLayout(const TObjectPtr<USaveSlot>& InSaveSlot)
{
	const FCharacterAbilities CharacterChosenAbilities = InSaveSlot->GetCharacterChosenAbilities(Character);
	bool bFirstSlot = true;
	for (const int32 AbilityIndex : CharacterChosenAbilities.Abilities)
	{
		SetupAbilitySlot(AbilityIndex, bFirstSlot, bFirstSlot ? FirstAbilitySlot : SecondAbilitySlot);
		bFirstSlot = false;
	}
}

void UCharacterAbilitiesUI::SaveAbilitiesLayout(const TObjectPtr<USaveSlot>& InSaveSlot)
{
	const int32 FirstAbilityIndex = FirstAbilitySlot ? FirstAbilitySlot->GetAbilityIndex() : GetAvailableAbilityIndex(InSaveSlot);
	AddAbility(FirstAbilityIndex);
	const int32 SecondAbilityIndex = SecondAbilitySlot ? SecondAbilitySlot->GetAbilityIndex() : GetAvailableAbilityIndex(InSaveSlot);
	AddAbility(SecondAbilityIndex);
	InSaveSlot->AddChosenAbilities(Character, FirstAbilityIndex, SecondAbilityIndex);
}

void UCharacterAbilitiesUI::SetAbilitiesIconData()
{
	for (int32 i = 0; i < Abilities.Num(); i++) if (AbilitiesData.IsValidIndex(i)) Abilities[i]->SetData(Character, AbilitiesData[i]);
}

int32 UCharacterAbilitiesUI::GetAvailableAbilityIndex(const TObjectPtr<USaveSlot>& InSaveSlot)
{
	const int32 UnlockedAbilitiesAmount = InSaveSlot->GetCharacterUnlockedAbilities(Character).Abilities.Num() + 1;
	for (int32 i = 0; i < UnlockedAbilitiesAmount; i++)
	{
		if (!Abilities[i]->GetIsLocked() && !Abilities[i]->GetIsInSlot()) return i;
	}
	return -1;
}

void UCharacterAbilitiesUI::NativeConstruct()
{
	Super::NativeConstruct();
	Abilities = { FirstAbility, SecondAbility, ThirdAbility, FourthAbility };
	FirstAbility->GetButton()->OnClicked.AddDynamic(this, &UCharacterAbilitiesUI::FirstAbilitySelected);
	SecondAbility->GetButton()->OnClicked.AddDynamic(this, &UCharacterAbilitiesUI::SecondAbilitySelected);
	ThirdAbility->GetButton()->OnClicked.AddDynamic(this, &UCharacterAbilitiesUI::ThirdAbilitySelected);
}

void UCharacterAbilitiesUI::SetupAbilitySlot(const int32 InAbilityIndex, const bool bIsFirstSlot, TObjectPtr<UAbilityIconUI>& InAbilitySlot)
{
	TObjectPtr<UWidgetAnimation> Animation;
	switch (InAbilityIndex)
	{
		case 0:
			Animation = bIsFirstSlot ? Select1To1ReverseAnim : Select1To2ReverseAnim;
			InAbilitySlot = FirstAbility;
			break;
		case 1:
			Animation = bIsFirstSlot ? Select2To1ReverseAnim : Select2To2ReverseAnim;
			InAbilitySlot = SecondAbility;
			break;
		default: return;
	}
	PlayAnimation(Animation);
	StopAnimation(Animation);
	InAbilitySlot->SetIsInSlot(true);
}

void UCharacterAbilitiesUI::AddAbility(const int32 InAbilityIndex)
{
	switch (InAbilityIndex)
	{
		case 0: FirstAbilitySelected(); break;
		case 1: SecondAbilitySelected();
		default: ;
	}
}

void UCharacterAbilitiesUI::FirstAbilitySelected()
{
	if (FirstAbility->GetIsInSlot()) return;
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::AbilitySelect);
	FirstAbility->SetIsInSlot(true);
	if (!FirstAbilitySlot)
	{
		FirstAbilitySlot = FirstAbility;
		PlayAnimationForward(Select1To1Anim);
	}
	else if (!SecondAbilitySlot)
	{
		SecondAbilitySlot = FirstAbility;
		PlayAnimationForward(Select1To2Anim);
	}
	else FirstAbility->PlayWobbleAnim();
}

void UCharacterAbilitiesUI::SecondAbilitySelected()
{
	if (SecondAbility->GetIsLocked())
	{
		(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnOpenUnlockAbilityPopup.ExecuteIfBound(1);
		return;
	}
	if (SecondAbility->GetIsInSlot()) return;
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::AbilitySelect);
	SecondAbility->SetIsInSlot(true);
	if (!FirstAbilitySlot)
	{
		FirstAbilitySlot = SecondAbility;
		PlayAnimationForward(Select2To1Anim);
	}
	else if (!SecondAbilitySlot)
	{
		SecondAbilitySlot = SecondAbility;
		PlayAnimationForward(Select2To2Anim);
	}
	else SecondAbility->PlayWobbleAnim();
}

void UCharacterAbilitiesUI::ThirdAbilitySelected()
{
	if (ThirdAbility->GetIsLocked())
	{
		(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnOpenUnlockAbilityPopup.ExecuteIfBound(2);
		return;
	}
}

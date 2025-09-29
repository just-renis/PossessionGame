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
	else if (FirstAbilitySlot == ThirdAbility) PlayAnimationForward(Select3To1ReverseAnim);
	FirstAbilitySlot->SetIsInSlot(false);
	FirstAbilitySlot = nullptr;
}

void UCharacterAbilitiesUI::ResetSecondAbilitySlot()
{
	if (!SecondAbilitySlot) return;
	if (SecondAbilitySlot == FirstAbility) PlayAnimationForward(Select1To2ReverseAnim);
	else if (SecondAbilitySlot == SecondAbility) PlayAnimationForward(Select2To2ReverseAnim);
	else if (SecondAbilitySlot == ThirdAbility) PlayAnimationForward(Select3To2ReverseAnim);
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
	const int32 FirstAbilityIndex = FirstAbilitySlot ? FirstAbilitySlot->GetAbilityIndex() : GetAvailableAbilityIndex();
	AddAbility(FirstAbilityIndex);
	const int32 SecondAbilityIndex = SecondAbilitySlot ? SecondAbilitySlot->GetAbilityIndex() : GetAvailableAbilityIndex();
	AddAbility(SecondAbilityIndex);
	InSaveSlot->AddChosenAbilities(Character, FirstAbilityIndex, SecondAbilityIndex);
}

void UCharacterAbilitiesUI::SetAbilitiesIconData()
{
	for (int32 i = 0; i < Abilities.Num(); i++) if (AbilitiesData.IsValidIndex(i)) Abilities[i]->SetData(Character, AbilitiesData[i]);
}

int32 UCharacterAbilitiesUI::GetAvailableAbilityIndex()
{
	for (int32 i = 0; i < Abilities.Num(); i++) if (!Abilities[i]->GetIsLocked() && !Abilities[i]->GetIsInSlot()) return i;
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
		case 2:
			Animation = bIsFirstSlot ? Select3To1ReverseAnim : Select3To2ReverseAnim;
			InAbilitySlot = ThirdAbility;
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
		case 1: SecondAbilitySelected(); break;
		case 2: ThirdAbilitySelected();
		default: ;
	}
}

void UCharacterAbilitiesUI::FirstAbilitySelected()
{
	if (FirstAbility->GetIsInSlot()) return;
	if (!FirstAbilitySlot)
	{
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::AbilitySelect);
		FirstAbilitySlot = FirstAbility;
		PlayAnimationForward(Select1To1Anim);
		FirstAbility->SetIsInSlot(true);
	}
	else if (!SecondAbilitySlot)
	{
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::AbilitySelect);
		SecondAbilitySlot = FirstAbility;
		PlayAnimationForward(Select1To2Anim);
		FirstAbility->SetIsInSlot(true);
	}
	else FirstAbility->PlayWobbleAnim();
}

void UCharacterAbilitiesUI::SecondAbilitySelected()
{
	if (!AbilitiesData[1].bAbilityExists) return;
	if (SecondAbility->GetIsLocked())
	{
		(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnOpenUnlockAbilityPopup.ExecuteIfBound(1);
		return;
	}
	if (SecondAbility->GetIsInSlot()) return;
	if (!FirstAbilitySlot)
	{
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::AbilitySelect);
		FirstAbilitySlot = SecondAbility;
		PlayAnimationForward(Select2To1Anim);
		SecondAbility->SetIsInSlot(true);
	}
	else if (!SecondAbilitySlot)
	{
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::AbilitySelect);
		SecondAbilitySlot = SecondAbility;
		PlayAnimationForward(Select2To2Anim);
		SecondAbility->SetIsInSlot(true);
	}
	else SecondAbility->PlayWobbleAnim();
}

void UCharacterAbilitiesUI::ThirdAbilitySelected()
{
	if (!AbilitiesData[2].bAbilityExists) return;
	if (ThirdAbility->GetIsLocked())
	{
		(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnOpenUnlockAbilityPopup.ExecuteIfBound(2);
		return;
	}
	if (ThirdAbility->GetIsInSlot()) return;
	if (!FirstAbilitySlot)
	{
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::AbilitySelect);
		FirstAbilitySlot = ThirdAbility;
		PlayAnimationForward(Select3To1Anim);
		ThirdAbility->SetIsInSlot(true);
	}
	else if (!SecondAbilitySlot)
	{
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::AbilitySelect);
		SecondAbilitySlot = ThirdAbility;
		PlayAnimationForward(Select3To2Anim);
		ThirdAbility->SetIsInSlot(true);
	}
	else ThirdAbility->PlayWobbleAnim();
}

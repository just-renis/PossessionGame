#include "AbilityManager.h"

#include "PossessionGame/Abilities/Characters/Ranger/RangerAbility1.h"
#include "PossessionGame/Abilities/Characters/Ranger/RangerAbility2.h"
#include "PossessionGame/Abilities/Characters/Ranger/RangerAbility3.h"
#include "PossessionGame/Abilities/Characters/Spy/SpyAbility1.h"
#include "PossessionGame/Abilities/Characters/Spy/SpyAbility2.h"
#include "PossessionGame/Abilities/Characters/Spy/SpyAbility3.h"
#include "PossessionGame/Abilities/Characters/Trickster/TricksterAbility1.h"

TObjectPtr<UAbility> UAbilityManager::GetAbility(EAbility AbilityName, TObjectPtr<AActor> Proprietor)
{
    return NewObject<UAbility>(Proprietor, Abilities[AbilityName]->GetClass());
}

void UAbilityManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    InitAbilities();
}

void UAbilityManager::InitAbilities()
{
    AddAbility(EAbility::SpyAbility1, USpyAbility1::StaticClass());
    AddAbility(EAbility::SpyAbility2, USpyAbility2::StaticClass());
    AddAbility(EAbility::SpyAbility3, USpyAbility3::StaticClass());
    AddAbility(EAbility::RangerAbility1, URangerAbility1::StaticClass());
    AddAbility(EAbility::RangerAbility2, URangerAbility2::StaticClass());
    AddAbility(EAbility::RangerAbility3, URangerAbility3::StaticClass());
    AddAbility(EAbility::TricksterAbility1, UTricksterAbility1::StaticClass());
   // AddAbility(EAbility::TricksterAbility2, URangerAbility2::StaticClass());
   // AddAbility(EAbility::TricksterAbility3, URangerAbility3::StaticClass());
}

void UAbilityManager::AddAbility(EAbility AbilityName, TSubclassOf<UAbility> AbilityClass)
{
    Abilities.Add(AbilityName, NewObject<UAbility>(this, AbilityClass));
}
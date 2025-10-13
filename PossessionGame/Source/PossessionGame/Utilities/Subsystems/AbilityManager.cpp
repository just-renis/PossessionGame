#include "AbilityManager.h"

#include "PossessionGame/Abilities/Characters/Breacher/BreacherAbility1.h"
#include "PossessionGame/Abilities/Characters/Guardian/GuardianAbility1.h"
#include "PossessionGame/Abilities/Characters/Guardian/GuardianAbility2.h"
#include "PossessionGame/Abilities/Characters/Guardian/GuardianAbility3.h"
#include "PossessionGame/Abilities/Characters/MinionBoss/MinionBossAbility1.h"
#include "PossessionGame/Abilities/Characters/MinionBoss/MinionBossAbility2.h"
#include "PossessionGame/Abilities/Characters/Ranger/RangerAbility1.h"
#include "PossessionGame/Abilities/Characters/Ranger/RangerAbility2.h"
#include "PossessionGame/Abilities/Characters/Ranger/RangerAbility3.h"
#include "PossessionGame/Abilities/Characters/Spy/SpyAbility1.h"
#include "PossessionGame/Abilities/Characters/Spy/SpyAbility2.h"
#include "PossessionGame/Abilities/Characters/Spy/SpyAbility3.h"
#include "PossessionGame/Abilities/Characters/Trickster/TricksterAbility1.h"
#include "PossessionGame/Abilities/Characters/Trickster/TricksterAbility2.h"
#include "PossessionGame/Abilities/Characters/Trickster/TricksterAbility3.h"
#include "PossessionGame/Abilities/Characters/Warden/WardenAbility1.h"
#include "PossessionGame/Enums/ECharacterType.h"

EAbility UAbilityManager::GetAbilityName(const ECharacterType InCharacterName, const int32 InAbilityIndex)
{
    if (FCharacterAbilitiesClasses* CharacterAbilitiesClasses = AbilitiesForCharacters.Find(InCharacterName))
    {
        if (const FAbilityClass* AbilityClass = CharacterAbilitiesClasses->AbilitiesClasses.Find(InAbilityIndex)) return AbilityClass->AbilityName;
    }
    return EAbility::None;
}

TObjectPtr<UAbility> UAbilityManager::GetAbility(const TObjectPtr<AActor>& InProprietor, const ECharacterType InCharacterName, const int32 InAbilityIndex)
{
    if (FCharacterAbilitiesClasses* CharacterAbilitiesClasses = AbilitiesForCharacters.Find(InCharacterName))
    {
        if (const FAbilityClass* AbilityClass = CharacterAbilitiesClasses->AbilitiesClasses.Find(InAbilityIndex)) return NewObject<UAbility>(InProprietor, *AbilityClass->AbilityClass);
    }
    return nullptr;
}

void UAbilityManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    InitAbilities();
}

void UAbilityManager::InitAbilities()
{
    AddAbility(ECharacterType::Spy, 0, EAbility::SpyAbility1, USpyAbility1::StaticClass());
    AddAbility(ECharacterType::Spy, 1, EAbility::SpyAbility2, USpyAbility2::StaticClass());
    AddAbility(ECharacterType::Spy, 2, EAbility::SpyAbility3, USpyAbility3::StaticClass());
    AddAbility(ECharacterType::Ranger, 0, EAbility::RangerAbility1, URangerAbility1::StaticClass());
    AddAbility(ECharacterType::Ranger, 1, EAbility::RangerAbility2, URangerAbility2::StaticClass());
    AddAbility(ECharacterType::Ranger, 2, EAbility::RangerAbility3, URangerAbility3::StaticClass());
    AddAbility(ECharacterType::Trickster, 0, EAbility::TricksterAbility1, UTricksterAbility1::StaticClass());
    AddAbility(ECharacterType::Trickster, 1, EAbility::TricksterAbility2, UTricksterAbility2::StaticClass());
    AddAbility(ECharacterType::Trickster, 2, EAbility::TricksterAbility3, UTricksterAbility3::StaticClass());
    AddAbility(ECharacterType::Guardian, 0, EAbility::GuardianAbility1, UGuardianAbility1::StaticClass());
    AddAbility(ECharacterType::Guardian, 1, EAbility::GuardianAbility2, UGuardianAbility2::StaticClass());
    AddAbility(ECharacterType::Guardian, 2, EAbility::GuardianAbility3, UGuardianAbility3::StaticClass());
    AddAbility(ECharacterType::MinionBoss, 0, EAbility::MinionBossAbility1, UMinionBossAbility1::StaticClass());
    AddAbility(ECharacterType::MinionBoss, 1, EAbility::MinionBossAbility2, UMinionBossAbility2::StaticClass());
    AddAbility(ECharacterType::Warden, 0, EAbility::WardenAbility1, UWardenAbility1::StaticClass());
    AddAbility(ECharacterType::Breacher, 0, EAbility::BreacherAbility1, UBreacherAbility1::StaticClass());
}

void UAbilityManager::AddAbility(const ECharacterType InCharacterName, const int32 InAbilityIndex, const EAbility InAbilityName, const TSubclassOf<UAbility>& InAbilityClass)
{
    FCharacterAbilitiesClasses& CharacterAbilitiesClasses = AbilitiesForCharacters.FindOrAdd(InCharacterName);
    CharacterAbilitiesClasses.AbilitiesClasses.Add(InAbilityIndex, FAbilityClass(InAbilityName, InAbilityClass));
}
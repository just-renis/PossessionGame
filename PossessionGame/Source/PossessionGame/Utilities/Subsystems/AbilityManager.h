#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Enums/EAbility.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "AbilityManager.generated.h"

enum class ECharacterType : uint8;
class UAbility;

USTRUCT()
struct FAbilityClass
{
    GENERATED_BODY()

    EAbility AbilityName = EAbility::None;
    UPROPERTY()
    TSubclassOf<UAbility> AbilityClass;
};

USTRUCT()
struct FCharacterAbilitiesClasses
{
    GENERATED_BODY()

    UPROPERTY()
    TMap<int32, FAbilityClass> AbilitiesClasses;
};

UCLASS()
class POSSESSIONGAME_API UAbilityManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
    EAbility GetAbilityName(const ECharacterType InCharacterName, const int32 InAbilityIndex);
    TObjectPtr<UAbility> GetAbility(const TObjectPtr<AActor>& InProprietor, const ECharacterType InCharacterName, const int32 InAbilityIndex);
private:
    UPROPERTY()
    TMap<ECharacterType, FCharacterAbilitiesClasses> AbilitiesForCharacters;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    void InitAbilities();
    void AddAbility(const ECharacterType InCharacterName, const int32 InAbilityIndex, const EAbility InAbilityName, const TSubclassOf<UAbility>& InAbilityClass);
};
#pragma once

#include "CoreMinimal.h"
#include "AbilityManager.generated.h"

enum class EAbility : uint8;
enum class ECharacterType : uint8;
class UAbility;

USTRUCT()
struct POSSESSIONGAME_API FCharacterAbilities
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<EAbility> Abilities;
};

UCLASS()
class POSSESSIONGAME_API UAbilityManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
    EAbility GetAbilityFromCharacterName(const ECharacterType InCharacterName, const int32 InAbilityIndex);
    TObjectPtr<UAbility> GetAbility(EAbility AbilityName, TObjectPtr<AActor> Proprietor);

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
private:
    UPROPERTY()
    TMap<ECharacterType, FCharacterAbilities> AbilitiesForCharacters;
    UPROPERTY()
    TMap<EAbility, TObjectPtr<UAbility>> Abilities;

    void InitAbilities();
    void AddAbility(EAbility AbilityName, TSubclassOf<UAbility> AbilityClass);
};
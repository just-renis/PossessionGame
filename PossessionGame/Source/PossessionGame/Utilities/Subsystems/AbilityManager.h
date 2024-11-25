#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Enums/EAbility.h"
#include "AbilityManager.generated.h"

class UAbility;

UCLASS()
class POSSESSIONGAME_API UAbilityManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
    TObjectPtr<UAbility> GetAbility(EAbility AbilityName, TObjectPtr<AActor> Proprietor);

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
private:
    UPROPERTY()
    TMap<EAbility, TObjectPtr<UAbility>> Abilities;

    void InitAbilities();
    void AddAbility(EAbility AbilityName, TSubclassOf<UAbility> AbilityClass);
};
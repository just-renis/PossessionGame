#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PossessionGame/Data Assets/Base Data Assets/AbilityData.h"
#include "PA_CharacterContainer.generated.h"

enum class EEnemyAnimState : uint8;
enum class EAbility : uint8;
class UAICharacterData;
class UPlayerCharacterData;
class AEnemy;

UCLASS()
class POSSESSIONGAME_API UPA_CharacterContainer : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Character Data")
	FName CharacterName;
	UPROPERTY(EditAnywhere, Category = "Character Data")
	TSoftClassPtr<AEnemy> CharacterClass;
	UPROPERTY(EditAnywhere, Category = "Character Data")
	TSoftObjectPtr<USkeletalMesh> CharacterMesh;
	UPROPERTY(EditAnywhere, Category = "Character Animation Montages")
	TMap<EEnemyAnimState, TSoftObjectPtr<UAnimMontage>> Montages;
	UPROPERTY(EditAnywhere, Category = "Character Data")
	TSoftObjectPtr<UAICharacterData> AI_Data;
	UPROPERTY(EditAnywhere, Category = "Character Data")
	TSoftObjectPtr<UPlayerCharacterData> P_Data;
	UPROPERTY(EditAnywhere, Category = "Abilities Data")
	TMap<EAbility, TSoftObjectPtr<UAbilityData>> AI_Abilities_Data;
	UPROPERTY(EditAnywhere, Category = "Abilities Data")
	TMap<EAbility, TSoftObjectPtr<UAbilityData>> P_Abilities_Data;
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};

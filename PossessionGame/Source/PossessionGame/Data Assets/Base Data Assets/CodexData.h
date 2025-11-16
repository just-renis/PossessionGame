#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CodexData.generated.h"

class UCharacterData;
class UCodexProjectileData;
class UThrowableData;
enum class EPoolObject : uint8;
class UDestructibleData;
class UAbilityData;
enum class EAbility : uint8;
enum class ECharacterType : uint8;

UCLASS()
class POSSESSIONGAME_API UCodexData : public UDataAsset
{
	GENERATED_BODY()
public:
	TObjectPtr<UCharacterData> GetCharacterData(const bool bIsPlayer, const ECharacterType InCharacterName);
	TObjectPtr<UAbilityData> GetAbilityData(const bool bIsPlayer, const EAbility InAbilityName);
	TObjectPtr<UDestructibleData> GetDestructibleData(const bool bIsPlayer, const EPoolObject InDestructibleName);
	TObjectPtr<UThrowableData> GetThrowableData(const bool bIsPlayer, const EPoolObject InThrowableName);
	TObjectPtr<UCodexProjectileData> GetProjectileData(const bool bIsPlayer, const EPoolObject InProjectileName);
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TMap<ECharacterType, TObjectPtr<UCharacterData>> EnemyCharactersHp;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TMap<ECharacterType, TObjectPtr<UCharacterData>> PlayerCharactersHp;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TMap<EAbility, TObjectPtr<UAbilityData>> EnemyAbilitiesData;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TMap<EAbility, TObjectPtr<UAbilityData>> PlayerAbilitiesData;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TMap<EPoolObject, TObjectPtr<UDestructibleData>> EnemyDestructiblesData;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TMap<EPoolObject, TObjectPtr<UDestructibleData>> PlayerDestructiblesData;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TMap<EPoolObject, TObjectPtr<UThrowableData>> EnemyThrowablesData;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TMap<EPoolObject, TObjectPtr<UThrowableData>> PlayerThrowablesData;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TMap<EPoolObject, TObjectPtr<UCodexProjectileData>> EnemyProjectilesData;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TMap<EPoolObject, TObjectPtr<UCodexProjectileData>> PlayerProjectilesData;
};

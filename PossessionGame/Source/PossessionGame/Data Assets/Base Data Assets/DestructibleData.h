#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PossessionGame/Utilities/Data Utilities/FVariationVfxParams.h"
#include "DestructibleData.generated.h"

enum class EVariation : uint8;
enum class EPoolObject : uint8;

UCLASS()
class POSSESSIONGAME_API UDestructibleData : public UDataAsset
{
	GENERATED_BODY()
public:
	bool IsVariationValuesExist() const { return !VariationValues.IsEmpty(); }
	bool GetIsLifeSpanImmediatelyAdded() const { return bIsLifeSpanImmediatelyAdded; }
	bool GetIsAffectedByPhysics() const { return bIsAffectedByPhysics; }
	float GetLifeSpan() const { return LifeSpan; }
	float GetDamageReduction() const { return DamageReduction; }
	float GetHp() const { return Hp; }
	EPoolObject GetObjectName() const { return ObjectName; }
	FName GetCollisionProfileName(const bool bIsPlayerObject) const { return bIsPlayerObject ? PlayerCollisionProfileName : EnemyCollisionProfileName; }
	
    void SetupVariationMaterials(const TArray<TObjectPtr<UMaterialInstanceDynamic>>& InMaterials, const EVariation InVariation);
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsLifeSpanImmediatelyAdded;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsAffectedByPhysics;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float LifeSpan;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float DamageReduction;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float Hp;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	EPoolObject ObjectName;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FName PlayerCollisionProfileName;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FName EnemyCollisionProfileName;
	UPROPERTY(EditDefaultsOnly, Category = "Variation Values", meta = (AllowPrivateAccess = true))
	TMap<EVariation, FMaterialVariationVfxParams> VariationValues;
};

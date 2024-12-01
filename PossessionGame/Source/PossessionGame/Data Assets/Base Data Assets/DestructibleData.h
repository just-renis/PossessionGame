#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DestructibleData.generated.h"

enum class EPoolObject : uint8;

UCLASS()
class POSSESSIONGAME_API UDestructibleData : public UDataAsset
{
	GENERATED_BODY()
public:
	bool GetIsLifeSpanImmediatelyAdded() const { return bIsLifeSpanImmediatelyAdded; }
	bool GetIsAffectedByPhysics() const { return bIsAffectedByPhysics; }
	float GetLifeSpan() const { return LifeSpan; }
	float GetHp() const { return Hp; }
	EPoolObject GetObjectName() const { return ObjectName; }
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsLifeSpanImmediatelyAdded;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsAffectedByPhysics;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float LifeSpan;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float Hp;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	EPoolObject ObjectName;
};

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
	bool GetIsLifeSpanImmediatelyAdded() { return bIsLifeSpanImmediatelyAdded; }
	bool GetIsAffectedByPhysics() { return bIsAffectedByPhysics; }
	float GetLifeSpan() { return LifeSpan; }
	float GetHp() { return Hp; }
	EPoolObject GetObjectName() { return ObjectName; }
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

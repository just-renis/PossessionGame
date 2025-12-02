#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterData.generated.h"

UCLASS()
class POSSESSIONGAME_API UCharacterData : public UDataAsset
{
	GENERATED_BODY()
public:
	virtual float GetMaxHp() { return MaxHp; }
	float GetMaxWalkSpeed() const { return MaxWalkSpeed; }
	float GetMaxRunSpeed() const { return MaxRunSpeed; }
	float GetBashImpulseForce() const { return BashImpulseForce; }
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float MaxHp;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float MaxWalkSpeed;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float MaxRunSpeed;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float BashImpulseForce = 8000.f;
};

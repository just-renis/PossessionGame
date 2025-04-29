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
	float GetMaxSlowedSpeed() const { return MaxSlowedSpeed; }
	float GetDashCooldown() const { return DashCooldown; }
	float GetDashStrength() const { return DashStrength; }
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float MaxHp;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float MaxWalkSpeed;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float MaxRunSpeed;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float MaxSlowedSpeed;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float DashCooldown;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float DashStrength;
};

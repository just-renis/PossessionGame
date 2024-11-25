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
	float GetMaxWalkSpeed() { return MaxWalkSpeed; }
	float GetMaxRunSpeed() { return MaxRunSpeed; }
	float GetMaxSlowedSpeed() { return MaxSlowedSpeed; }
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float MaxHp;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float MaxWalkSpeed;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float MaxRunSpeed;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float MaxSlowedSpeed;
};

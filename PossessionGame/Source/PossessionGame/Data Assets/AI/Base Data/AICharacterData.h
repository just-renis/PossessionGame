#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Data Assets/Base Data Assets/CharacterData.h"
#include "AICharacterData.generated.h"

UCLASS()
class POSSESSIONGAME_API UAICharacterData : public UCharacterData
{
	GENERATED_BODY()
public:
	bool GetIsPossessable() const { return bIsPossessable; }
	float GetShootPause() const { return ShootPause; }
	float GetPossessionTimeValue() const { return PossessionTimeValue; }
	int32 GetXp() const { return Xp; }
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsPossessable = true;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float ShootPause = 0.2f;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float PossessionTimeValue = 11.25f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 Xp = 0;
};

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
	float GetMaxRunSpeed() const { return MaxRunSpeed; }
	float GetEliteHpIncreasePercentage() const { return HpIncreasePercentage; }
	float GetEliteDamageIncreasePercentage() const { return DamageIncreasePercentage; }
	int32 GetXp() const { return Xp; }
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsPossessable = true;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float ShootPause = 0.2f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float MaxRunSpeed;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 Xp = 0;
	UPROPERTY(EditAnywhere, Category = "Elite", meta = (AllowPrivateAccess = true))
	float HpIncreasePercentage = 30.f;
	UPROPERTY(EditAnywhere, Category = "Elite", meta = (AllowPrivateAccess = true))
	float DamageIncreasePercentage = 30.f;
};

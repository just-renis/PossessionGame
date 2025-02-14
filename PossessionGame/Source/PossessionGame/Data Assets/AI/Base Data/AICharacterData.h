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
	float GetEmpEffectTime() const { return EmpEffectTime; }
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsPossessable = true;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float ShootPause = 0.2f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float EmpEffectTime;
};

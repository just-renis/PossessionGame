#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Data Assets/Base Data Assets/CharacterData.h"
#include "PlayerCharacterData.generated.h"

UCLASS()
class POSSESSIONGAME_API UPlayerCharacterData : public UCharacterData
{
	GENERATED_BODY()
public:
	virtual float GetMaxHp() override { return bIsGodModeEnabled ? 100000000.f : Super::GetMaxHp(); }
	float GetPossessionBarCooldown() const { return bIsGodModeEnabled ? 2.f : PossessionBarCooldown; }
	float GetDashCooldown() const { return DashCooldown; }
	float GetDashStrength() const { return DashStrength; }
	
	void SetIsGodModeEnabled(bool bState) { bIsGodModeEnabled = bState; }
private:
	bool bIsGodModeEnabled = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float PossessionBarCooldown;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float DashCooldown;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float DashStrength;
};

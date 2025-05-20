#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Data Assets/Base Data Assets/GameModeData.h"
#include "ChargeTerminalModeData.generated.h"

UCLASS()
class POSSESSIONGAME_API UChargeTerminalModeData : public UGameModeData
{
	GENERATED_BODY()
public:
	float GetTerminalHackRequired() const { return TerminalHackRequired; }
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float TerminalHackRequired = 5.f;
};

#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Data Assets/Base Data Assets/GameModeData.h"
#include "ChargeTerminalModeData.generated.h"

UCLASS()
class POSSESSIONGAME_API UChargeTerminalModeData : public UGameModeData
{
	GENERATED_BODY()
public:
	int32 GetUsbAmountRequired() const { return UsbAmountRequired; }
	int32 GetChargeAmountRequired() const { return ChargeAmountRequired; }
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	int32 UsbAmountRequired = 2;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	int32 ChargeAmountRequired = 3;
};

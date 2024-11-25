#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilityData.generated.h"

UCLASS()
class POSSESSIONGAME_API UAbilityData : public UDataAsset
{
	GENERATED_BODY()
public:
	int32 GetRefreshableAbilityIndex() { return RefreshableAbilityIndex; }
	int32 GetCurrentCooldownKill() { return CurrentCooldownKill; }
	float GetAbilityDuration() { return AbilityDuration; }
	float GetRefreshAmount() { return RefreshAmount; }
	float GetCooldownSecs() { return CooldownSecs; }
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 CurrentCooldownKill;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 RefreshableAbilityIndex = -1;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float AbilityDuration;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float RefreshAmount;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float CooldownSecs;
};

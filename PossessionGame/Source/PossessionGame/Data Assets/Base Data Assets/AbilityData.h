#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilityData.generated.h"

enum class EAbility : uint8;

UCLASS()
class POSSESSIONGAME_API UAbilityData : public UDataAsset
{
	GENERATED_BODY()
public:
	EAbility GetAbilityName() const { return AbilityName; }
	int32 GetRefreshableAbilityIndex() const { return RefreshableAbilityIndex; }
	int32 GetCurrentCooldownKill() const { return CurrentCooldownKill; }
	float GetAbilityDuration() const { return AbilityDuration; }
	float GetRefreshAmount() const { return RefreshAmount; }
	float GetCooldownSecs() const { return CooldownSecs; }
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	EAbility AbilityName;
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

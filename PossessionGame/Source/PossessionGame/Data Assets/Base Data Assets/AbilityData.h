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
	bool GetIsAbilityWithDuration() const { return bHasDuration; }
	int32 GetRefreshableAbilityIndex() const { return RefreshableAbilityIndex; }
	int32 GetCurrentCooldownKill() const { return CurrentCooldownKill; }
	int32 GetAbilityIndex() const { return AbilityIndex; }
	int32 GetChargesAmount() const { return ChargesAmount; }
	float GetAbilityDuration() const { return AbilityDuration; }
	float GetRefreshAmount() const { return RefreshAmount; }
	float GetCooldownSecs() const { return CooldownSecs; }
	EAbility GetAbilityName() const { return AbilityName; }
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	EAbility AbilityName;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bHasDuration = false;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bHasDuration", EditConditionHides))
	float AbilityDuration;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 CurrentCooldownKill;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 RefreshableAbilityIndex = -1;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 AbilityIndex;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 ChargesAmount;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float RefreshAmount;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float CooldownSecs;
};

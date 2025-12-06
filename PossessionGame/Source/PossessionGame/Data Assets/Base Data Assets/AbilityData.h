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
	bool GetIsAbilityWithDuration() const { return bHasDuration; }
	float GetAbilityDuration() const { return AbilityDuration; }
	float GetBaseDamageReduction() const { return BaseDamageReduction; }
	float GetCooldownSecs() const { return CooldownSecs; }
	float GetImpulseStrength() const { return ImpulseStrength; }
	float GetSpecificStrength() const { return SpecificStrength; }
	int32 GetAbilityIndex() const { return AbilityIndex; }
	int32 GetChargesAmount() const { return ChargesAmount; }
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	EAbility AbilityName;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bHasDuration = false;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bHasDuration", EditConditionHides))
	float AbilityDuration;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bHasDamageReduction = false;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bHasDamageReduction", EditConditionHides))
	float BaseDamageReduction;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bHasImpulseStrength = false;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bHasImpulseStrength", EditConditionHides))
	float ImpulseStrength = 10.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bHasSpecificStrength = false;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bHasSpecificStrength", EditConditionHides))
	float SpecificStrength = 1.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 AbilityIndex;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 ChargesAmount;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float CooldownSecs;
};

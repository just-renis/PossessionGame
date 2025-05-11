#pragma once

#include "CoreMinimal.h"
#include "Ability.generated.h"

enum class EAbility : uint8;
class UDelegateManager;
class UAbilityData;
class AEnemy;

UCLASS()
class POSSESSIONGAME_API UAbility : public UObject
{
	GENERATED_BODY()
public:
	EAbility GetAbilityName() const;
	bool CanExecute() const { return bIsCooldownKill ? CurrentCooldownKill >= ReqCooldownKill : CurrentChargesAmount >= 1; }
	bool GetIsCooldownKill() const { return bIsCooldownKill; }
	bool GetIsAbilityInDuration() const { return bIsAbilityInDuration; }
	bool GetIsAbilityCharged() const { return CurrentChargesAmount > 0; }
	bool GetIsMaxCharged() const { return CurrentChargesAmount >= MaxChargesAmount; }
	int32 GetCurrentCooldownKill() const { return CurrentCooldownKill; }
	int32 GetReqCooldownKill() const { return ReqCooldownKill; }
	int32 GetAbilityIndex() const;
	int32 GetCurrentCharges() const { return CurrentChargesAmount; }
	float GetReqCooldownSecs() const { return ReqCooldownSecs; }
	float GetCurrentCooldownSecs() const { return CurrentCooldownSecs; }
	float GetCurrentDuration() const { return CurrentDuration; }

	virtual void AI_Execute() { UseCharge(); }
	virtual void AI_StopAbility() { ResetDuration(); }

	virtual void P_Execute();
	virtual void P_StopAbility() { ResetDuration(); }

	virtual void Tick(const float InDeltaTime);
	void ReduceKillCooldown() { CurrentCooldownKill++; }
	void Setup(TObjectPtr<AEnemy> InAbilityOwner, TObjectPtr<UAbilityData> InAbilityData, TObjectPtr<UDelegateManager> InDelegateManager);
	void SetupAdditionalProperties(const bool bIsPlayer, TObjectPtr<UAbility> InAbilityToRefresh);
	void ApplyUpgradeOnSelection(const bool bIsPlayer);
protected:
	bool bIsAbilityInDuration = false;
	int32 CurrentChargesAmount = 0;
	int32 MaxChargesAmount;
	float CurrentDuration;
	float CurrentCooldownSecs;
	float DurationTime;
	float ReqCooldownSecs;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;
	
	virtual void P_ApplyVariation() {}

	virtual bool IsProprietorPossessed() { return false; }
	
	virtual void SetProprietor(TObjectPtr<AEnemy> InAbilityProprietor) {}
	void UseCharge();
	virtual void CooldownLogic(const float InDeltaTime);
	virtual void SetupUpgradeAppliance(const bool bIsPlayer);
private:
	bool bIsCooldownKill = false;
	bool bIsRefreshingOtherAbility = false;
	bool bIsDurational = false;
	int32 CurrentCooldownKill;
	int32 ReqCooldownKill;
	float RefreshOtherAbilityAmount;
	UPROPERTY()
	TObjectPtr<UAbilityData> Data;
	UPROPERTY()
	TObjectPtr<UAbility> RefreshableAbility;

	void ReduceCooldownTimer(const float InRefreshAmount) { CurrentCooldownSecs += InRefreshAmount; }
	void RefreshOtherAbility() const;
	void SetupDuration(const float InAbilityDuration, const bool bIsPlayer);
	void SetupRefreshOtherAbility(const float InRefreshAmount, TObjectPtr<UAbility> InAbilityToRefresh);
	void ResetDuration();
	void AbilityInDurationLogic(const float InDeltaTime);
};
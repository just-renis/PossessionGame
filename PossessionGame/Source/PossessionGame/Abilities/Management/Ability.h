#pragma once

#include "CoreMinimal.h"
#include "Ability.generated.h"

class UDelegateManager;
class UAbilityData;
class AEnemy;

UCLASS()
class POSSESSIONGAME_API UAbility : public UObject
{
	GENERATED_BODY()
public:
	bool CanExecute() const { return bIsCooldownKill ? CurrentCooldownKill >= ReqCooldownKill : CurrentCooldownSecs >= ReqCooldownSecs; }
	bool GetIsCooldownKill() const { return bIsCooldownKill; }
	bool GetIsAbilityInDuration() const { return bIsAbilityInDuration; }
	int GetCurrentCooldownKill() const { return CurrentCooldownKill; }
	int GetReqCooldownKill() const { return ReqCooldownKill; }
	float GetReqCooldownSecs() const { return ReqCooldownSecs; }
	float GetCurrentCooldownSecs() const { return CurrentCooldownSecs; }
	float GetCurrentDuration() const { return CurrentDuration; }

	virtual void AI_Execute() { ResetCooldown(); }
	virtual void AI_StopAbility() { ResetDuration(); }

	virtual void P_Execute() { ResetCooldown(); }
	virtual void P_StopAbility() { ResetDuration(); }

	virtual void Tick(float DeltaTime);
	void ReduceKillCooldown() { CurrentCooldownKill++; }
	void Setup(TObjectPtr<AEnemy> AbilityOwner, TObjectPtr<UAbilityData> AbilityData, TObjectPtr<UDelegateManager> InDelegateManager);
	void SetupAdditionalProperties(TObjectPtr<UAbilityData> AbilityData, TObjectPtr<UAbility> AbilityToRefresh);
protected:
	bool bIsAbilityInDuration = false;
	float CurrentDuration;
	float CurrentCooldownSecs;
	float DurationTime;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;

	virtual bool IsProprietorPossessed() { return false; }
	
	virtual void SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) {}
	void ResetCooldown();
	virtual void CooldownLogic(const float DeltaTime);
private:
	bool bIsCooldownKill = false;
	bool bIsRefreshingOtherAbility = false;
	bool bIsDurational = false;
	int32 CurrentCooldownKill;
	int32 ReqCooldownKill;
	float RefreshOtherAbilityAmount;
	float ReqCooldownSecs;
	UPROPERTY()
	TObjectPtr<UAbility> RefreshableAbility;

	void ReduceCooldownTimer(float RefreshAmount) { CurrentCooldownSecs += RefreshAmount; }
	void RefreshOtherAbility();
	void SetupDuration(float AbilityDuration);
	void SetupRefreshOtherAbility(float RefreshAmount, TObjectPtr<UAbility> AbilityToRefresh);
	void ResetDuration();
	void AbilityInDurationLogic(float DeltaTime);
};
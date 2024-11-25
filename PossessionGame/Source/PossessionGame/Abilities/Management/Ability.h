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
	bool CanExecute() { return bIsCooldownKill ? CurrentCooldownKill <= 0 : CurrentCooldownSecs <= 0.f; }
	bool GetIsCooldownKill() { return bIsCooldownKill; }
	bool GetIsAbilityInDuration() { return bIsAbilityInDuration; }
	int GetCurrentCooldownKill() { return CurrentCooldownKill; }
	float GetCurrentCooldownSecs() { return CurrentCooldownSecs; }
	float GetCurrentDuration() { return CurrentDuration; }

	virtual void AI_Execute() { ResetCooldown(); }
	virtual void AI_StopAbility() { ResetDuration(); }

	virtual void P_Execute() { ResetCooldown(); }
	virtual void P_StopAbility() { ResetDuration(); }

	virtual void Tick(float DeltaTime);
	void ReduceKillCooldown() { CurrentCooldownKill--; }
	void Setup(TObjectPtr<AEnemy> AbilityOwner, TObjectPtr<UAbilityData> AbilityData, TObjectPtr<UDelegateManager> InDelegateManager);
	void SetupAdditionalProperties(TObjectPtr<UAbilityData> AbilityData, TObjectPtr<UAbility> AbilityToRefresh);
protected:
	bool bIsAbilityInDuration = false;
	float CurrentDuration;
	float DurationTime;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;

	virtual bool IsProprietorPossessed() { return false; }
	
	virtual void SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) {}
	void ResetCooldown();
private:
	bool bIsCooldownKill = false;
	bool bIsRefreshingOtherAbility = false;
	bool bIsDurational = false;
	int32 CurrentCooldownKill;
	int32 ReqCooldownKill;
	float RefreshOtherAbilityAmount;
	float CurrentCooldownSecs;
	float ReqCooldownSecs;
	UPROPERTY()
	TObjectPtr<UAbility> RefreshableAbility;

	void ReduceCooldownTimer(float RefreshAmount) { CurrentCooldownSecs -= RefreshAmount; }
	void RefreshOtherAbility();
	void SetupDuration(float AbilityDuration);
	void SetupRefreshOtherAbility(float RefreshAmount, TObjectPtr<UAbility> AbilityToRefresh);
	void ResetDuration();
	void AbilityInDurationLogic(float DeltaTime);
};
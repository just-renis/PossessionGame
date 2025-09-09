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
	bool CanExecute() const { return CurrentChargesAmount >= 1; }
	bool GetIsAbilityInDuration() const { return bIsAbilityInDuration; }
	bool GetIsAbilityCharged() const { return CurrentChargesAmount > 0; }
	bool GetIsMaxCharged() const { return CurrentChargesAmount >= MaxChargesAmount; }
	int32 GetAbilityIndex() const { return AbilityIndex; }
	int32 GetCurrentCharges() const { return CurrentChargesAmount; }
	float GetReqCooldownSecs() const { return ReqCooldownSecs; }
	float GetCurrentCooldownSecs() const { return CurrentCooldownSecs; }

	virtual void AI_Execute();
	virtual void AI_StopAbility() { ResetDuration(); }

	void P_InitiateExecute();
	virtual void P_StopAbility();

	virtual void Tick(const float InDeltaTime);
	virtual void Setup(TObjectPtr<AEnemy> InAbilityOwner, TObjectPtr<UAbilityData> InAbilityData, TObjectPtr<UDelegateManager> InDelegateManager);
	virtual void SetupUpgradeAppliance(const bool bIsPlayer = true);
	void SetAbilityIndex(const int32 InAbilityIndex) { AbilityIndex = InAbilityIndex; }
protected:
	bool bIsAbilityInDuration = false;
	int32 CurrentChargesAmount = 0;
	int32 MaxChargesAmount;
	float CurrentDuration;
	float CurrentCooldownSecs;
	float DurationTime;
	float ReqCooldownSecs;
	UPROPERTY()
	TObjectPtr<UAbilityData> Data;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;

	virtual float AI_GetCooldownReduction();

	virtual float P_GetCooldownReduction();
	
	virtual void P_ApplyVariation() {}
	virtual void P_Execute();

	virtual bool IsProprietorPossessed() { return false; }
	
	virtual void SetProprietor(TObjectPtr<AEnemy> InAbilityProprietor) {}
	void UseCharge() { CurrentChargesAmount--; }
	virtual void CooldownLogic(const float InDeltaTime);
private:
	bool bIsDurational = false;
	int32 AbilityIndex;

	void ReduceCooldownTimer(const float InRefreshAmount) { CurrentCooldownSecs += InRefreshAmount; }
	void ResetDuration();
	void AbilityInDurationLogic(const float InDeltaTime);
};
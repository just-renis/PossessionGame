#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PlayerDataManager.generated.h"

enum class EUpgradeAttribute : uint8;
enum class EUpgrade : uint8;

DECLARE_DELEGATE_OneParam(FOnRangerAbilityStackStatusChanged, const bool);

USTRUCT()
struct FTricksterPurpleVariationData
{
	GENERATED_BODY()
	
	float Uptime = 0.f;
	float UptimeRequired = 1.f;
	float StackDuration = 15.f;
	float DamageBoostPercentage = 0.1f;
	TArray<float> Stacks;

	float GetDamageBoost() const { return DamageBoostPercentage * Stacks.Num(); }
	
	void AddPoisonAreaUptime(const float InUptime)
	{
		Uptime += InUptime;
		if (Uptime >= UptimeRequired)
		{
			Uptime -= UptimeRequired;
			Stacks.Add(0.f);
		}
	}
	void TickStackDuration(const float InTickTime)
	{
		for (int32 i = Stacks.Num() - 1; i >= 0; i--)
		{
			Stacks[i] += InTickTime;
			if (Stacks[i] >= StackDuration) Stacks.RemoveAt(i);
		}
	}
};

USTRUCT()
struct FRangerBlueVariationData
{
	GENERATED_BODY()
	
	float StackDuration = 20.f;
	float DamageBoostPercentage = 0.08f;
	TArray<float> Stacks;

	float GetCriticalDamageBoost() const { return DamageBoostPercentage * Stacks.Num(); }
	
	void AddStack() { Stacks.Add(0.f); }
	void TickStackDuration(const float InTickTime)
	{
		for (int32 i = Stacks.Num() - 1; i >= 0; i--)
		{
			Stacks[i] += InTickTime;
			if (Stacks[i] >= StackDuration) Stacks.RemoveAt(i);
		}
	}
};

USTRUCT()
struct FRangerGrenadeSpecialData
{
	GENERATED_BODY()
	
	float StackDuration = 15.f;
	float DamageBoostPercentage = 0.1f;
    FOnRangerAbilityStackStatusChanged OnRangerAbilityStackStatusChanged;
	TArray<float> Stacks;

	float GetDamageBoost() const { return DamageBoostPercentage * Stacks.Num(); }
	
	void AddStack()
	{
		if (Stacks.IsEmpty()) (void)OnRangerAbilityStackStatusChanged.ExecuteIfBound(true);
		Stacks.Add(0.f);
	}
	void RemoveStackAt(const int32 InIndex)
	{
		Stacks.RemoveAt(InIndex);
		if (Stacks.IsEmpty()) (void)OnRangerAbilityStackStatusChanged.ExecuteIfBound(false);
	}
	void TickStackDuration(const float InTickTime)
	{
		for (int32 i = Stacks.Num() - 1; i >= 0; i--)
		{
			Stacks[i] += InTickTime;
			if (Stacks[i] >= StackDuration) RemoveStackAt(i);
		}
	}
};

USTRUCT()
struct FRangerShieldSpecialData
{
	GENERATED_BODY()
	
	float StackDuration = 15.f;
	float DamageBoostPercentage = 0.01f;
	TArray<float> Stacks;

	float GetDamageBoost() const { return DamageBoostPercentage * Stacks.Num(); }
	
	void AddStack() { Stacks.Add(0.f); }
	void RemoveStackAt(const int32 InIndex) { Stacks.RemoveAt(InIndex); }
	void TickStackDuration(const float InTickTime)
	{
		for (int32 i = Stacks.Num() - 1; i >= 0; i--)
		{
			Stacks[i] += InTickTime;
			if (Stacks[i] >= StackDuration) RemoveStackAt(i);
		}
	}
};

USTRUCT()
struct FGuardianTimeStopSpecialData
{
	GENERATED_BODY()
	
	float StackDuration = 20.f;
	float DamageBoostPercentage = 0.05f;
	TArray<float> Stacks;

	float GetDamageBoost() const { return DamageBoostPercentage * Stacks.Num(); }
	
	void AddStack() { Stacks.Add(0.f); }
	void RemoveStackAt(const int32 InIndex) { Stacks.RemoveAt(InIndex); }
	void TickStackDuration(const float InTickTime)
	{
		for (int32 i = Stacks.Num() - 1; i >= 0; i--)
		{
			Stacks[i] += InTickTime;
			if (Stacks[i] >= StackDuration) RemoveStackAt(i);
		}
	}
};

UCLASS()
class POSSESSIONGAME_API UPlayerDataManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	float GetPoisonAreaStackBoost() const { return TricksterPurpleVariationData.GetDamageBoost(); }
	float GetRangerGrenadeSpecialBoost() const { return RangerGrenadeSpecialData.GetDamageBoost(); }
	float GetRangerShieldSpecialBoost() const { return RangerShieldSpecialData.GetDamageBoost(); }
	float GetGuardianTimeStopSpecialBoost() const { return GuardianTimeStopSpecialData.GetDamageBoost(); }
	float GetCriticalDamageStackBoost() const { return RangerBlueVariationData.GetCriticalDamageBoost(); }
	FOnRangerAbilityStackStatusChanged& GetOnRangerAbilityStackStatusChangedDelegate() { return RangerGrenadeSpecialData.OnRangerAbilityStackStatusChanged; }
	
	void AddPoisonAreaUptime(const float InUptime) { TricksterPurpleVariationData.AddPoisonAreaUptime(InUptime); }
	void AddCriticalDamageStack() { RangerBlueVariationData.AddStack(); }
	void AddRangerGrenadeSpecialStack() { RangerGrenadeSpecialData.AddStack(); }
	void AddRangerShieldSpecialStack() { RangerShieldSpecialData.AddStack(); }
	void AddGuardianTimeStopSpecialStack() { GuardianTimeStopSpecialData.AddStack(); }
	void Reset();
	void Tick(const float InTickTime);
private:
	FTricksterPurpleVariationData TricksterPurpleVariationData;
	FRangerGrenadeSpecialData RangerGrenadeSpecialData;
	FRangerShieldSpecialData RangerShieldSpecialData;
	FGuardianTimeStopSpecialData GuardianTimeStopSpecialData;
	FRangerBlueVariationData RangerBlueVariationData;
};

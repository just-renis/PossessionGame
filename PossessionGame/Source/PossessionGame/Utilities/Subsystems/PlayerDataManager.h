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
	float StackDuration = 12.f;
	float DamageBoostPercentage = 0.05f;
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
	
	float StackDuration = 14.f;
	float DamageBoostPercentage = 0.04f;
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
	
	float StackDuration = 15.f;
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

USTRUCT()
struct FRecentHijackBoost
{
	GENERATED_BODY()

	bool bIsEnabled = false;
	bool bIsFourthOptionA = false;
	float Duration = 0.f;
	float BoostPercentage = 0.f;
	float CurrentDuration = 0.f;

	bool GetIsEnabled() const { return bIsEnabled; }
	float GetBoost() const { return BoostPercentage; }
	
	void AddBoost(const bool bInIsFourthOptionA)
	{
		bIsFourthOptionA = bInIsFourthOptionA;
		CurrentDuration = 0.f;
		Duration = bInIsFourthOptionA ? 6.f : 4.f;
		BoostPercentage = bInIsFourthOptionA ? 0.35f : 0.6f;
		bIsEnabled = true;
	}
	void RemoveBoost()
	{
		bIsEnabled = false;
		BoostPercentage = bIsFourthOptionA ? 0.f : 1.f;
	}
	void TickDuration(const float InTickTime)
	{
		CurrentDuration += InTickTime;
		if (CurrentDuration >= Duration) RemoveBoost();
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
	float GetRangerBlueVariationStackBoost() const { return RangerBlueVariationData.GetCriticalDamageBoost(); }
	float GetRecentHijackBoost() const { return RecentHijackBoostData.GetBoost(); }
	FOnRangerAbilityStackStatusChanged& GetOnRangerAbilityStackStatusChangedDelegate() { return RangerGrenadeSpecialData.OnRangerAbilityStackStatusChanged; }
	
	void AddPoisonAreaUptime(const float InUptime) { TricksterPurpleVariationData.AddPoisonAreaUptime(InUptime); }
	void AddCriticalDamageStack() { RangerBlueVariationData.AddStack(); }
	void AddRangerGrenadeSpecialStack() { RangerGrenadeSpecialData.AddStack(); }
	void AddRangerShieldSpecialStack() { RangerShieldSpecialData.AddStack(); }
	void AddGuardianTimeStopSpecialStack() { GuardianTimeStopSpecialData.AddStack(); }
	void AddRecentHijackBoost(const bool bInIsFourthOptionA) { RecentHijackBoostData.AddBoost(bInIsFourthOptionA); }
	void Reset();
	void Tick(const float InTickTime);
private:
	FTricksterPurpleVariationData TricksterPurpleVariationData;
	FRangerGrenadeSpecialData RangerGrenadeSpecialData;
	FRangerShieldSpecialData RangerShieldSpecialData;
	FGuardianTimeStopSpecialData GuardianTimeStopSpecialData;
	FRangerBlueVariationData RangerBlueVariationData;
	FRecentHijackBoost RecentHijackBoostData;
};

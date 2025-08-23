#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PlayerDataManager.generated.h"

enum class EUpgradeAttribute : uint8;
enum class EUpgrade : uint8;

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

UCLASS()
class POSSESSIONGAME_API UPlayerDataManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	float GetPoisonAreaStackBoost() const { return TricksterPurpleVariationData.GetDamageBoost(); }
	
	void AddPoisonAreaUptime(const float InUptime) { TricksterPurpleVariationData.AddPoisonAreaUptime(InUptime); }
	void Reset();
	void Tick(const float InTickTime);
private:
	FTricksterPurpleVariationData TricksterPurpleVariationData;
};

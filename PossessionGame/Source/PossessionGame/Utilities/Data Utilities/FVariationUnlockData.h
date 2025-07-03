#pragma once

#include "FVariationUnlockData.generated.h"

enum class EVariation : uint8;
enum class ECharacterType : uint8;

USTRUCT()
struct FVariationUnlockGoal
{
	GENERATED_BODY()

	explicit FVariationUnlockGoal(const int32 InRequirement, const int32 InCoinRequirement) : Requirement(InRequirement), CoinRequirement(InCoinRequirement), bIsBought(false), Current(0) {}
	FVariationUnlockGoal() : Requirement(0), CoinRequirement(0), bIsBought(false), Current(0) {}

	int32 Requirement;
	int32 CoinRequirement;
	UPROPERTY()
	bool bIsBought;
	UPROPERTY()
	int32 Current;
};

USTRUCT()
struct FVariationUnlockData
{
	GENERATED_BODY()

	FVariationUnlockData();

	bool IsVariationUnlocked(const EVariation InVariationUnlockName);
	bool IsVariationTaskCompleted(const EVariation InVariationUnlockName) const;
	int32 GetVariationUnlockProgress(const EVariation InVariationUnlockName) const;
	int32 GetVariationUnlockRequirement(const EVariation InVariationUnlockName) const;
	EVariation GetSelectedVariation(const ECharacterType InCharacter) const;
	
	void ProgressVariationUnlock(const EVariation InVariationUnlockName, const int32 InAmount);
	void UpdateSelectedVariation(const ECharacterType InCharacterType, const EVariation InVariationName);
private:
	UPROPERTY()
	TMap<ECharacterType, EVariation> SelectedVariations;
	UPROPERTY()
	TMap<EVariation, FVariationUnlockGoal> VariationUnlocks;
};

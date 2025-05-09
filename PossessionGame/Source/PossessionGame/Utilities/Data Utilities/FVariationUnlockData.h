#pragma once

#include "FVariationUnlockData.generated.h"

enum class EVariation : uint8;
enum class ECharacterType : uint8;
enum class EVariationUnlock : uint8;

USTRUCT()
struct FVariationUnlockGoal
{
	GENERATED_BODY()

	explicit FVariationUnlockGoal(const int32 InRequirement) : Requirement(InRequirement), Current(0) {}
	FVariationUnlockGoal() : Requirement(0), Current(0) {}

	int32 Requirement;
	int32 Current;
};

USTRUCT()
struct FVariationUnlockData
{
	GENERATED_BODY()

	FVariationUnlockData();

	bool IsVariationUnlocked(const EVariationUnlock InVariationUnlockName) const;
	int32 GetVariationUnlockProgress(const EVariationUnlock InVariationUnlockName) const;
	int32 GetVariationUnlockRequirement(const EVariationUnlock InVariationUnlockName) const;
	EVariation GetSelectedVariation(const ECharacterType InCharacter) const;
	
	void ProgressVariationUnlock(const EVariationUnlock InVariationUnlockName);
	void UpdateSelectedVariation(const ECharacterType InCharacterType, const EVariation InVariationName);
private:
	UPROPERTY()
	TMap<ECharacterType, EVariation> SelectedVariations;
	UPROPERTY()
	TMap<EVariationUnlock, FVariationUnlockGoal> VariationUnlocks;
};

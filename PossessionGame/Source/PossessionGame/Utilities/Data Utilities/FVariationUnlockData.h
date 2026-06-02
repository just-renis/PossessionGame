#pragma once

#include "PossessionGame/Enums/EVariation.h"
#include "FVariationUnlockData.generated.h"

enum class EVariation : uint8;
enum class ECharacterType : uint8;

USTRUCT()
struct FUnlockedVariations
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<EVariation> UnlockedVariations;
	UPROPERTY()
	EVariation Current = EVariation::Default;
};

USTRUCT()
struct FVariationUnlockData
{
	GENERATED_BODY()

	FVariationUnlockData();

	bool IsVariationUnlocked(const EVariation InVariationUnlockName);
	EVariation GetSelectedVariation(const ECharacterType InCharacter) const;
	
	void UpdateSelectedVariation(const ECharacterType InCharacterType, const EVariation InVariationName);
	void UnlockVariation(const EVariation InVariationName);
private:
	UPROPERTY()
	TMap<ECharacterType, FUnlockedVariations> SelectedVariations;
};

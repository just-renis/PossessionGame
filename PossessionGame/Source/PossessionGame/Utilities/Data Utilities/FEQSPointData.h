#pragma once

#include "FEQSPointData.generated.h"

enum class ECharacterType : uint8;

USTRUCT()
struct POSSESSIONGAME_API FEQSPointData
{
	GENERATED_BODY()

	bool bIsOccupied;
	FVector Location;
	TSet<ECharacterType> ViableCharacters;

	FEQSPointData() : bIsOccupied(false), Location(FVector::ZeroVector) {}
	FEQSPointData(const FVector& InLocation, const TSet<ECharacterType>& InViableCharacters) : bIsOccupied(false), Location(InLocation), ViableCharacters(InViableCharacters) {}
};

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CollectibleData.generated.h"

enum class EPoolObject : uint8;
enum class EVfx : uint8;

UCLASS()
class POSSESSIONGAME_API UCollectibleData : public UDataAsset
{
	GENERATED_BODY()
public:
	bool GetIsFloating() const { return bIsFloating; }
	bool GetIsMarkerEnabled() const { return bIsMarkerEnabled; }
	bool HasAura() const { return bHasAura; }
	FRotator GetRotation() const { return Rotation; }
	EVfx GetAuraName() const { return AuraName; }
	EPoolObject GetCollectibleName() const { return CollectibleName; }
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsFloating = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsMarkerEnabled = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bHasAura = false;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	FRotator Rotation = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bHasAura", EditConditionHides))
	EVfx AuraName;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	EPoolObject CollectibleName;
};

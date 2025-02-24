#pragma once

#include "CoreMinimal.h"
#include "UpgradeData.generated.h"

enum class EUpgradeAttribute : uint8;
enum class EUpgrade : uint8;

UCLASS()
class POSSESSIONGAME_API UUpgradeData : public UDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	EUpgrade UpgradeName;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TMap<EUpgradeAttribute, float> UpgradeAttributeValues;
};

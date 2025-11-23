#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MiscData.generated.h"

UCLASS()
class POSSESSIONGAME_API UMiscData : public UDataAsset
{
	GENERATED_BODY()
public:
	float GetDamage() const { return Damage; }
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float Damage = 0.f;
};

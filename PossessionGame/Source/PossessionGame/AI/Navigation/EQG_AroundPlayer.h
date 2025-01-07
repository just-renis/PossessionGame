#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/Generators/EnvQueryGenerator_Donut.h"
#include "EQG_AroundPlayer.generated.h"

UCLASS()
class POSSESSIONGAME_API UEQG_AroundPlayer : public UEnvQueryGenerator
{
	GENERATED_BODY()
public:
	UEQG_AroundPlayer();
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float RangerMetresDistance = 4.f;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float DefaultMetresDistance = 4.f;
	
	virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const override;
};

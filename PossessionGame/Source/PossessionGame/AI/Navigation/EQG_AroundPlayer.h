#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/Generators/EnvQueryGenerator_Donut.h"
#include "EQG_AroundPlayer.generated.h"

UCLASS()
class POSSESSIONGAME_API UEQG_AroundPlayer : public UEnvQueryGenerator
{
	GENERATED_BODY()

	UEQG_AroundPlayer();
	
	virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const override;
};

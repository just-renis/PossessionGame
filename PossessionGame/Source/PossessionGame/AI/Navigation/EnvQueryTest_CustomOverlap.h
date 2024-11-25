#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_CustomOverlap.generated.h"

UCLASS(MinimalAPI, DisplayName = "Custom Overlap Test")
class UEnvQueryTest_CustomOverlap : public UEnvQueryTest
{
	GENERATED_BODY()

	UEnvQueryTest_CustomOverlap(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, Category = "Overlap")
	FEnvOverlapData OverlapData;

	virtual FText GetDescriptionTitle() const override;
	
	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
};

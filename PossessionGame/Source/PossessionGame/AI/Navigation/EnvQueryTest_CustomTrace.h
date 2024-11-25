#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_CustomTrace.generated.h"

class UEnvQueryTest_Overlap;

UCLASS(MinimalAPI, DisplayName = "Custom Trace Test")
class UEnvQueryTest_CustomTrace : public UEnvQueryTest
{
	GENERATED_BODY()
	
	UEnvQueryTest_CustomTrace(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	TSubclassOf<UEnvQueryContext> Context;
	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	FAIDataProviderBoolValue TraceFromContext;
	UPROPERTY(EditDefaultsOnly, Category = "Trace", AdvancedDisplay)
	FAIDataProviderFloatValue ItemHeightOffset;
	UPROPERTY(EditDefaultsOnly, Category = "Trace", AdvancedDisplay)
	FAIDataProviderFloatValue ContextHeightOffset;

	virtual FText GetDescriptionTitle() const override;
	
	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
};

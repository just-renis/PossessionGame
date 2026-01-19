#pragma once

#include "CoreMinimal.h"
#include "Throwable.h"
#include "WraithMeteorTrigger.generated.h"

UCLASS()
class POSSESSIONGAME_API AWraithMeteorTrigger : public AThrowable
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MI_MeshBaseMaterial;

	virtual void BeginPlay() override;
};

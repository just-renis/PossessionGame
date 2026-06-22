#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WallLaser.generated.h"

UCLASS()
class POSSESSIONGAME_API AWallLaser : public AActor
{
	GENERATED_BODY()

	AWallLaser();
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_Material;

	virtual void BeginPlay() override;
};

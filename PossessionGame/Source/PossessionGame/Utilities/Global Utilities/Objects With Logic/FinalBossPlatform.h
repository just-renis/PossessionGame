#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FinalBossPlatform.generated.h"

UCLASS()
class POSSESSIONGAME_API AFinalBossPlatform : public AActor
{
	GENERATED_BODY()

	AFinalBossPlatform();

	FTimerHandle RisingHandle;
	float UnitsRose = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "Rising", meta = (AllowPrivateAccess = true))
	float UnitsToRise = 500.f;
	UPROPERTY(EditDefaultsOnly, Category = "Mesh", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;

	virtual void BeginPlay() override;
	void SetupRisingLogic();
	void ProcessRising();
};

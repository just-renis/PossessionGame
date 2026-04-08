#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerminalBlockade.generated.h"

UCLASS()
class POSSESSIONGAME_API ATerminalBlockade : public AActor
{
	GENERATED_BODY()
	
	ATerminalBlockade();
	
	float UnitsToRise = 530.f;
	float UnitsRose = 0.f;
	FTimerHandle RisingHandle;
	UPROPERTY(EditAnywhere, Category = "Id", meta = (AllowPrivateAccess = true))
	int32 Id;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float RisingSpeed = 5.f;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;

	virtual void BeginPlay() override;
	void Interact(const int32 InId);
	void ProcessRising();
};

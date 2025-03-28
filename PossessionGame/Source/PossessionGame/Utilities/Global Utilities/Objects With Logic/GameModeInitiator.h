#pragma once

#include "CoreMinimal.h"
#include "GameModeInitiator.generated.h"

enum class EGameMode : uint8;
class USphereComponent;

UCLASS()
class POSSESSIONGAME_API AGameModeInitiator : public AActor
{
	GENERATED_BODY()

	AGameModeInitiator();

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	EGameMode GameMode;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> Collision;

	virtual void BeginPlay() override;
	UFUNCTION()
	void Initiate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

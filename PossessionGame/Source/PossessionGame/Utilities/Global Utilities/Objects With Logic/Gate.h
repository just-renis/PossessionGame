#pragma once

#include "CoreMinimal.h"
#include "Gate.generated.h"

enum class EGameMode : uint8;
class UBoxComponent;

UCLASS()
class POSSESSIONGAME_API AGate : public AActor
{
	GENERATED_BODY()

	AGate();

	float TimePassed = 0.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsTutorialGate = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	EGameMode NextGameMode;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBoxComponent> Collision;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> FirstGateSide;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> SecondGateSide;

	virtual void BeginPlay() override;
	void OpenGate();
	UFUNCTION()
	void MoveGateSides();
	UFUNCTION()
	void GatePassed(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

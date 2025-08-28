#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerminalDevice.generated.h"

enum class EGameMode : uint8;
class UPointLightComponent;
class APointLight;

UCLASS()
class POSSESSIONGAME_API ATerminalDevice : public AActor
{
	GENERATED_BODY()

	ATerminalDevice();

	float CurrentIntensity = 50.f;
	UPROPERTY(EditDefaultsOnly, Category = "Mesh", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	EGameMode TerminalHackRoomMode;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UPointLightComponent> Light;
	UPROPERTY()
	FTimerHandle ProcessLightIntensityHandle;

	virtual void BeginPlay() override;
	void SetupProcessTimer(const EGameMode InTerminalHackRoomMode);
	void ProcessLightIntensity();
};

#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "Gate.generated.h"

class UWidgetComponent;
class UInteractableUI;
enum class EGameMode : uint8;
class UBoxComponent;

UCLASS()
class POSSESSIONGAME_API AGate : public AActor
{
	GENERATED_BODY()

	AGate();

	bool bEventToCloseGateHappened = false;
	bool bIsGateOpened = false;
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
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> InteractableWidget;

	virtual void BeginPlay() override;
	void OpenGate();
	void CloseGate(const EGameMode InGameMode = EGameMode::None);
	UFUNCTION()
	void OpenGateSides();
	UFUNCTION()
	void CloseGateSides();
};

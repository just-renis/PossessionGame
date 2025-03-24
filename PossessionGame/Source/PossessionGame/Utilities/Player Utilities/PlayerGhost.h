#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PlayerGhost.generated.h"

class ATutorialDrone;
class UDelegateManager;
class UPlayerGhostUI;
class UInputAction;
class UCameraComponent;
class UInputMappingContext;
class UPlayerHUD;

UCLASS()
class POSSESSIONGAME_API APlayerGhost : public ACharacter
{
	GENERATED_BODY()
public:
	APlayerGhost();
private:
	int32 CurrentTutorialStep = -1;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCameraComponent> Camera;
	UPROPERTY(EditAnywhere, Category = "Input", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputMappingContext> PlayerMappingContext;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<UPlayerGhostUI> PlayerGhostUIClass;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<ATutorialDrone> TutorialDroneClass;
	UPROPERTY()
	TObjectPtr<UPlayerGhostUI> PlayerGhostUI;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;
	UPROPERTY()
	TObjectPtr<AActor> Target;

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void Move(const FInputActionValue& InValue);
	void Rotate(const FInputActionValue& InValue);
	void StartRunning();
	void StopRunning();
	void PossessEnemy();
	void RestartLevel() const;
	void HandleEscapeClick();
	void SetCurrentTutorialStep(const int32 InStep) { CurrentTutorialStep = InStep; }
	void CompleteTutorialStep() { SetCurrentTutorialStep(-1); }
	UFUNCTION()
	void CheckIfPlayerFallOutOfWorld() const;
	UFUNCTION()
	void EnablePlayerInput() const;
};
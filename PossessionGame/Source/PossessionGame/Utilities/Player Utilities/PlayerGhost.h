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
	bool bTutorialModeEnabled = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCameraComponent> Camera;
	UPROPERTY(EditAnywhere, Category = "Input", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputMappingContext> PlayerMappingContext;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;

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
	void SetupTutorial();
	UFUNCTION()
	void CheckIfPlayerFallOutOfWorld() const;
	UFUNCTION()
	void EnablePlayerInput() const;
};
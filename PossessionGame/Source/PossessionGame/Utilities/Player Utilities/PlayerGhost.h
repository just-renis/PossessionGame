#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PlayerGhost.generated.h"

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
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> CharacterMovementComponent;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCameraComponent> Camera;
	UPROPERTY(EditAnywhere, Category = "Input", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputMappingContext> PlayerMappingContext;

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void Move(const FInputActionValue& Value);
	void Rotate(const FInputActionValue& Value);
	void StartRunning();
	void StopRunning();
	void PossessEnemy();
	void RestartLevel();
	void HandleEscapeClick();
	UFUNCTION()
	void CheckIfPlayerFallOutOfWorld();
};
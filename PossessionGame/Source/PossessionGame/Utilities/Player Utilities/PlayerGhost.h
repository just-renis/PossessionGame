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
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCameraComponent> Camera;
	UPROPERTY(EditAnywhere, Category = "Input", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputMappingContext> PlayerMappingContext;
	UPROPERTY()
	FTimerHandle CheckHandle;

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void Rotate(const FInputActionValue& InValue);
	void PossessEnemy();
	void RestartLevel() const;
	void HandleEscapeClick();
	void CheckIfPlayerFallOutOfWorld();
	void SetupUserSettings() const;
};
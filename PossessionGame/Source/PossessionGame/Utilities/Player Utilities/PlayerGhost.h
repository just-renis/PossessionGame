#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PlayerGhost.generated.h"

class AEnemy;
class UCameraComponent;
class UInputMappingContext;

UCLASS()
class POSSESSIONGAME_API APlayerGhost : public ACharacter
{
	GENERATED_BODY()
public:
	APlayerGhost();
private:
	FTimerHandle DetectIfPointingAtEnemyHandle;
	FTimerHandle CheckHandle;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCameraComponent> Camera;
	UPROPERTY(EditAnywhere, Category = "Input", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputMappingContext> PlayerMappingContext;

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Move(const FInputActionValue& Value);
	void Rotate(const FInputActionValue& InValue);
	void PossessEnemy();
	void RestartLevel() const;
	void HandleEscapeClick();
	void CheckIfPlayerFallOutOfWorld();
	void SetupUserSettings() const;
	void ToggleTimeDilation();
	void DetectIfPointingAtEnemy();
	void HijackNextChapterEnemyAutomatically(const TObjectPtr<AEnemy>& InHijackTarget);
};
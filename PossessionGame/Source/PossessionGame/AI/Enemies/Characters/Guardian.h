#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Guardian.generated.h"

UCLASS()
class POSSESSIONGAME_API AGuardian : public AEnemy
{
	GENERATED_BODY()
public:
	void SetCustomTimeDilation(const float InDilation);
private:
	AGuardian();

	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_Guardian;
	
	virtual void AI_SetupWeapon() override;
	virtual void AI_PlayDeathSound() override;
	virtual void AI_PlaySpawnSound() override;
	virtual void AI_PlayDeathElectrification() override;

	virtual void P_Setup() override;
	virtual void P_SetupLeftClickInputTriggeredState(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent) override {}
	virtual void P_SetupLeftClickInputStartedState(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent) override;
	virtual void P_SetupLeftClickInputCompletedState(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent) override;
	void P_OnShootStarted();
	void P_ResetShootHoldDuration() const;
	void P_CancelShootingHold();
	virtual void P_InitiateJump() override;
	virtual void P_InitiateStomp() override;
	virtual void P_InitiateDash() override;
	virtual void P_Move(const FInputActionValue& Value) override;

	virtual void BeginPlay() override;
	virtual void SetMesh(const ECharacterType PrimaryDataAssetName) override;
};

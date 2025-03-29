#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Guardian.generated.h"

UCLASS()
class POSSESSIONGAME_API AGuardian : public AEnemy
{
	GENERATED_BODY()

	AGuardian();
	
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> GuardianMaterial;
	
	virtual void AI_SetupWeapon() override;
	virtual void AI_PlayDeathSound() override;
	virtual void AI_PlaySpawnSound() override;

	virtual void P_Setup() override;
	virtual void P_SetupLeftClickInputTriggeredState(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent) override {}
	virtual void P_SetupLeftClickInputStartedState(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent) override;

	virtual void BeginPlay() override;
	virtual void SetMesh(const ECharacterType PrimaryDataAssetName) const override;
};

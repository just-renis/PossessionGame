#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Breacher.generated.h"

class UCapsuleComponent;

UCLASS()
class POSSESSIONGAME_API ABreacher : public AEnemy
{
	GENERATED_BODY()
public:
	void P_ThrowBarrel();
	void P_Bash(const float InImpulseStrength);
	void P_ThrowDynamite();
private:
	ABreacher();

	FTimerHandle StopBashHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Bash", meta = (AllowPrivateAccess = true))
	float BashStunDuration = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "Bash", meta = (AllowPrivateAccess = true))
	float DefaultBashDamage = 40.f;
	UPROPERTY(EditDefaultsOnly, Category = "Bash", meta = (AllowPrivateAccess = true))
	TObjectPtr<UCapsuleComponent> BashCollision;
	UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> BashCameraShake;
	UPROPERTY()
	TObjectPtr<UAudioComponent> DeathSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> SpawnSfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> BashVfx;
	
	virtual void AI_SetupWeapon() override;
	virtual void AI_TriggerDeathVoiceLine() override;

	virtual void P_Setup() override;
	void P_StopBash();
	virtual void P_SetupRightClickInput(const TObjectPtr<UEnhancedInputComponent> InEnhancedInputComponent) override;
	UFUNCTION()
	void P_BashHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void BeginPlay() override;
	void DetonateDynamites();
	UFUNCTION()
	void OnBashCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

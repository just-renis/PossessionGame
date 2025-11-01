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
	void P_Bash();
private:
	ABreacher();

	FTimerHandle StopDashHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Bash", meta = (AllowPrivateAccess = true))
	float BashStunDuration = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "Bash", meta = (AllowPrivateAccess = true))
	float BashStrength = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Bash", meta = (AllowPrivateAccess = true))
	TObjectPtr<UCapsuleComponent> BashCollision;
	UPROPERTY()
	TObjectPtr<UAudioComponent> DeathSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> SpawnSfx;
	
	virtual void AI_SetupWeapon() override;
	virtual void AI_TriggerDeathVoiceLine() override;

	virtual void P_Setup() override;
	void P_StopBash();

	virtual void BeginPlay() override;
	UFUNCTION()
	void OnBashCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

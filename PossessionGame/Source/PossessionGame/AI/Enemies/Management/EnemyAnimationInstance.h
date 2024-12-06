#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PossessionGame/Enums/EEnemyAnimState.h"
#include "EnemyAnimationInstance.generated.h"

class AEnemy;
class UPrimaryDataAssetsManager;

UCLASS()
class POSSESSIONGAME_API UEnemyAnimationInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	bool GetIsCrouching() { return bIsCrouching; }
	bool IsPlaying(EEnemyAnimState State);
	bool IsMontageExisting(EEnemyAnimState State);
	float GetMontageLength(EEnemyAnimState State);

	void SetIsCrouching(bool bState) { bIsCrouching = bState; }
	void SetIsTargetInSight(bool bState) { bIsTargetInSight = bState; }
	void SetAimPitch(float Value) { Pitch = Value; }
	void SetSpeed(float Value) { Speed = Value; }
	void SetDirectionalAngle();
	void PlayMontage(EEnemyAnimState State);
	void StopMontage(EEnemyAnimState State, float BlendOut = 0.f);
protected:
	UPROPERTY()
	TObjectPtr<AEnemy> Proprietor;
	UPROPERTY()
	TMap<EEnemyAnimState, TObjectPtr<UAnimMontage>> Montages;
	
	virtual void NativeBeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void AssignOnMontageEnd();
	void SetProprietor();
	virtual void InnerPlayMontage(EEnemyAnimState State);
	UFUNCTION()
	virtual void OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted);
private:
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bIsCrouching = false;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bIsTargetInSight = false;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	float Speed = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	float Pitch = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	float DirectionalAngle = 0.f;
	UPROPERTY()
    TObjectPtr<UPrimaryDataAssetsManager> AssetsManager;
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	void LoadMontageIfNotLoaded(EEnemyAnimState State);
};

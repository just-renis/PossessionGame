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
	bool GetIsCrouching() const { return bIsCrouching; }
	bool IsPlaying(EEnemyAnimState State);
	bool IsMontageExisting(EEnemyAnimState State);
	float GetMontageLength(EEnemyAnimState State);

	void SetIsCrouching(const bool bState) { bIsCrouching = bState; }
	void SetIsTargetInSight(const bool bState) { bIsTargetInSight = bState; }
	void SetSpeed(const float InValue) { Speed = InValue; }
	void SetLookAtLocation(const FVector& InLookAtLocation) { LookAtLocation = InLookAtLocation; }
	void SetDirectionalAngle();
	void PlayMontage(const EEnemyAnimState InState, const bool bPlayInReverse = false);
	void PlayMontageWithHold(const EEnemyAnimState InState, const bool bPlayInReverse, const float InHoldTime, const bool bDisableActionInMotion);
	void StopMontage(EEnemyAnimState State, float BlendOut = 0.f);
	void ClearTimers();
protected:
	UPROPERTY()
	TObjectPtr<AEnemy> Proprietor;
	UPROPERTY()
	TMap<EEnemyAnimState, TObjectPtr<UAnimMontage>> Montages;
	UPROPERTY()
	TObjectPtr<UPrimaryDataAssetsManager> AssetsManager;
	
	virtual void NativeBeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void AssignOnMontageEnd();
	void SetProprietor();
	virtual void InnerPlayMontage(const EEnemyAnimState InState, const bool bPlayInReverse);
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
	float DirectionalAngle = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	FVector LookAtLocation = FVector::ZeroVector;
	UPROPERTY()
	FTimerHandle MontageHoldEndHandle;
	UPROPERTY()
    FTimerHandle PauseHandle;
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	void LoadMontageIfNotLoaded(EEnemyAnimState State);
	void MontageHoldEnd(const bool bDisableActionInMotion) const;
};

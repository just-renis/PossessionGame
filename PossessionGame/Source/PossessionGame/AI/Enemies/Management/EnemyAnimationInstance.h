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
	bool IsPlaying(const EEnemyAnimState InState);
	bool IsMontageExisting(const EEnemyAnimState InState) const;
	float GetMontageLength(const EEnemyAnimState InState);

	void SetIsTargetInSight(const bool bState) { bIsTargetInSight = bState; }
	void SetSpeed(const float InValue) { Speed = InValue; }
	void SetLookAtLocation(const FVector& InLookAtLocation) { LookAtLocation = InLookAtLocation; }
	void SetDirectionalAngle();
	void PlayMontage(const EEnemyAnimState InState);
	void PlayMontageWithHold(const EEnemyAnimState InState, const float InHoldTime, const bool bNeedsToStopAILogic);
	void StopMontage(const EEnemyAnimState InState, const float InBlendOut = 0.f);
	virtual void ClearTimers();
protected:
	EEnemyAnimState MontageStateInHold;
	UPROPERTY()
	TObjectPtr<AEnemy> Proprietor;
	UPROPERTY()
	TMap<EEnemyAnimState, TObjectPtr<UAnimMontage>> Montages;
	UPROPERTY()
	TObjectPtr<UPrimaryDataAssetsManager> AssetsManager;
	
	virtual void NativeBeginPlay() override;
	virtual void NativeUninitializeAnimation() override;
	virtual void AssignOnMontageEnd();
	void SetProprietor();
	virtual void InnerPlayMontage(const EEnemyAnimState InState);
	virtual void MontageWithHoldEnd();
	UFUNCTION()
	virtual void OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted);
private:
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bIsTargetInSight = false;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	float Speed = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	float DirectionalAngle = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	FVector LookAtLocation = FVector::ZeroVector;
	UPROPERTY()
	FTimerHandle MontageWithHoldHandle;

	virtual void NativeUpdateAnimation(const float InDeltaSeconds) override;
	void LoadMontageIfNotLoaded(const EEnemyAnimState InState);
};

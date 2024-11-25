#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "HandsAnimInstance.generated.h"

class UDelegateManager;
class UPrimaryDataAssetsManager;
class AEnemy;

UCLASS()
class POSSESSIONGAME_API UHandsAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	bool GetIsReloading() { return bIsReloading; }
	bool IsPlaying(EHandsAnimState State);
	float GetMontagePosition(EHandsAnimState State);

	void SetSpeed(float Value) { Speed = Value; }
	void KeepPlayingMontage(EHandsAnimState State) { if (!IsPlaying(State)) PlayMontage(State); }
	void PlayMontage(EHandsAnimState State, float StartPosition = 0.f);
	void StopMontage(EHandsAnimState State, float BlendOut);
	void SetWeaponName(EWeapon Name) { WeaponName = Name; }
	void SetIsZoomedIn(bool bState) { bIsZoomedIn = bState; }
protected:
	UPROPERTY()
	TMap<EHandsAnimState, TObjectPtr<UAnimMontage>> Montages;
	
	virtual void NativeBeginPlay() override;
	UFUNCTION()
	virtual void OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted);
private:
	bool bIsReloading = false;
	EWeapon WeaponName;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bIsZoomedIn = false;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	float DirectionalAngle = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	float Speed = 0.f;
	UPROPERTY()
	TObjectPtr<UPrimaryDataAssetsManager> AssetsManager;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;

	bool IsMontageExisting(EHandsAnimState State);
	
	void LoadMontageIfNotLoaded(EHandsAnimState State);
	virtual void BeginDestroy() override;
};

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Enums/EWeaponAnimState.h"
#include "WeaponAnimInstance.generated.h"

class UPrimaryDataAssetsManager;

UCLASS()
class POSSESSIONGAME_API UWeaponAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	bool IsPlaying(EWeaponAnimState State);
	bool IsMontageExisting(EWeaponAnimState State);
	float GetMontageLength(EWeaponAnimState State);

	void SetSpeed(float Value) { Speed = Value; }
	void PlayMontage(EWeaponAnimState State, float StartPosition = 0.f);
	void StopMontage(EWeaponAnimState State, float BlendOut = 0.f);
	void SetWeaponName(EWeapon Name) { WeaponName = Name; }
	void SetGlobalPlayRate(const float InPlayRate);

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float Speed = 0.f;
protected:
	UPROPERTY()
	TMap<EWeaponAnimState, TObjectPtr<UAnimMontage>> Montages;
	
	UFUNCTION()
	virtual void OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted);
private:
	float GlobalPlayRate = 1.f;
	EWeapon WeaponName;
	UPROPERTY()
	TObjectPtr<UPrimaryDataAssetsManager> AssetsManager;
	
	virtual void NativeBeginPlay() override;
	void LoadMontageIfNotLoaded(EWeaponAnimState State);
	virtual void NativeUninitializeAnimation() override;
};
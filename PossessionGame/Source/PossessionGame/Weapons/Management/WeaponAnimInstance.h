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
	bool IsPlaying(const EWeaponAnimState InState);
	bool IsMontageExisting(const EWeaponAnimState InState) const;
	float GetMontageLength(const EWeaponAnimState InState);

	void SetSpeed(const float InValue) { Speed = InValue; }
	void PlayMontage(const EWeaponAnimState InState, const float InStartPosition = 0.f);
	void StopMontage(const EWeaponAnimState InState, const float InBlendOut = 0.f);
	void SetWeaponName(const EWeapon InName) { WeaponName = InName; }
	void SetGlobalPlayRate(const float InPlayRate);

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float Speed = 0.f;
protected:
	UPROPERTY()
	TMap<EWeaponAnimState, TObjectPtr<UAnimMontage>> Montages;

	bool IsMontage(const EWeaponAnimState InState, const UAnimMontage* InMontage) const;
	
	UFUNCTION()
	virtual void OnMontageEnd(UAnimMontage* InMontage, const bool bIsInterrupted) {}
private:
	float GlobalPlayRate = 1.f;
	EWeapon WeaponName;
	UPROPERTY()
	TObjectPtr<UPrimaryDataAssetsManager> AssetsManager;
	
	virtual void NativeBeginPlay() override;
	void LoadMontageIfNotLoaded(const EWeaponAnimState InState);
	virtual void NativeUninitializeAnimation() override;
};
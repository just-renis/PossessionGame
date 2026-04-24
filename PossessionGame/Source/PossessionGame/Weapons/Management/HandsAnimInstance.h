#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HandsAnimInstance.generated.h"

class AEnemy;
enum class EWeapon : uint8;
enum class EHandsAnimState : uint8;
class UDelegateManager;
class UPrimaryDataAssetsManager;

DECLARE_DELEGATE_OneParam(FOnHandsMontageEnd, bool);

UCLASS()
class POSSESSIONGAME_API UHandsAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	FOnHandsMontageEnd OnHandsMontageEnd;
	
	bool GetIsReloading() const { return bIsReloading; }
	bool IsPlaying(const EHandsAnimState InState);
	float GetMontagePosition(const EHandsAnimState InState);
	TObjectPtr<AEnemy> GetProprietor() const { return Proprietor; }

	void SetSpeed(const float InValue) { Speed = InValue; }
	void KeepPlayingMontage(const EHandsAnimState InState) { if (!IsPlaying(InState)) PlayMontage(InState); }
	void PlayMontage(const EHandsAnimState InState, const float InStartPosition = 0.f);
	void PlayMontageWithHold(const EHandsAnimState InState, const float InHoldTime);
	void StopMontage(const EHandsAnimState InState, const float InBlendOut);
	void SetWeaponName(const EWeapon InName) { WeaponName = InName; }
	virtual void SetProprietor(const TObjectPtr<AEnemy>& InProprietor) { Proprietor = InProprietor; }
	void SetGlobalPlayRate(const float InPlayRate);
protected:
	UPROPERTY()
	TObjectPtr<UPrimaryDataAssetsManager> AssetsManager;
	UPROPERTY()
	TObjectPtr<AEnemy> Proprietor;
	UPROPERTY()
	TMap<EHandsAnimState, TObjectPtr<UAnimMontage>> Montages;
	
	bool IsMontage(const EHandsAnimState InState, const UAnimMontage* InMontage) const;
	
	virtual void NativeBeginPlay() override;
	UFUNCTION()
	virtual void OnMontageEnd(UAnimMontage* InMontage, const bool bIsInterrupted);
private:
	bool bIsReloading = false;
	float GlobalPlayRate = 1.f;
	EWeapon WeaponName;
	FTimerHandle MontageWithHoldHandle;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bIsFalling = false;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	float DirectionalAngle = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	float Speed = 0.f;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;

	bool IsMontageExisting(const EHandsAnimState InState) const;
	
	void LoadMontageIfNotLoaded(const EHandsAnimState InState);
	virtual void NativeUpdateAnimation(const float InDeltaSeconds) override;
	void MontageWithHoldEnd();
	virtual void NativeUninitializeAnimation() override;
};

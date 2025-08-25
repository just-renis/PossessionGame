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
	void SetProprietor(const TObjectPtr<AEnemy>& InProprietor) { Proprietor = InProprietor; }
	void SetGlobalPlayRate(const float InPlayRate);
protected:
	UPROPERTY()
	TMap<EHandsAnimState, TObjectPtr<UAnimMontage>> Montages;
	
	virtual void NativeBeginPlay() override;
	UFUNCTION()
	virtual void OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted);
private:
	bool bIsReloading = false;
	float GlobalPlayRate = 1.f;
	EWeapon WeaponName;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	float DirectionalAngle = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	float Speed = 0.f;
	UPROPERTY()
	TObjectPtr<UPrimaryDataAssetsManager> AssetsManager;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;
	UPROPERTY()
	TObjectPtr<AEnemy> Proprietor;
	UPROPERTY()
	FTimerHandle MontageWithHoldHandle;

	bool IsMontageExisting(const EHandsAnimState InState);
	
	void LoadMontageIfNotLoaded(const EHandsAnimState InState);
	virtual void BeginDestroy() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	void MontageWithHoldEnd();
};

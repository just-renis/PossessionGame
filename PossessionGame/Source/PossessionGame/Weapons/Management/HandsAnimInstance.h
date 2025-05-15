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
	bool IsPlaying(EHandsAnimState State);
	float GetMontagePosition(EHandsAnimState State);
	TObjectPtr<AEnemy> GetProprietor() const { return Proprietor; }

	void SetSpeed(float Value) { Speed = Value; }
	void KeepPlayingMontage(EHandsAnimState State) { if (!IsPlaying(State)) PlayMontage(State); }
	void PlayMontage(EHandsAnimState State, float StartPosition = 0.f);
	void StopMontage(EHandsAnimState State, float BlendOut);
	void SetWeaponName(EWeapon Name) { WeaponName = Name; }
	void SetIsZoomedIn(bool bState) { bIsZoomedIn = bState; }
	void SetProprietor(const TObjectPtr<AEnemy>& InProprietor) { Proprietor = InProprietor; }
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
	UPROPERTY()
	TObjectPtr<AEnemy> Proprietor;

	bool IsMontageExisting(EHandsAnimState State);
	
	void LoadMontageIfNotLoaded(EHandsAnimState State);
	virtual void BeginDestroy() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};

#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "Throwable.generated.h"

enum class ESound : uint16;
class UThrowableData;

UCLASS()
class POSSESSIONGAME_API AThrowable : public APoolObject
{
	GENERATED_BODY()
public:
	bool GetInExplosionProcess() const { return bInExplosionProcess; }
	
	void Initialize(const TObjectPtr<AActor>& InProprietor);
	virtual void Throw();
	virtual void Disable(const bool bHide) override;
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {}
protected:
	AThrowable();
	
	bool bInExplosionProcess = false;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UThrowableData> Data;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY()
	TObjectPtr<AActor> Proprietor;
	UPROPERTY()
	TMap<ESound, TObjectPtr<UAudioComponent>> Sounds;

	virtual void OnImpact(const FHitResult& Hit);
	void PlaySound(const ESound InSoundName);
	virtual void SetupOnHitEvent();
};

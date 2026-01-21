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
	virtual void Throw();
	virtual void InitWithProprietor(const TObjectPtr<AEnemy> InProprietor) override;
	virtual void Disable(const bool bHide) override;
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {}
protected:
	AThrowable();
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UThrowableData> Data;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY()
	TObjectPtr<AEnemy> Proprietor;
	UPROPERTY()
	TMap<ESound, TObjectPtr<UAudioComponent>> Sounds;

	float GetPlayerLevelAllGlobalDamageBonus() const;

	virtual void OnImpact(const FHitResult& InHit);
	void PlaySound(const ESound InSoundName);
	virtual void SetupOnHitEvent();
};

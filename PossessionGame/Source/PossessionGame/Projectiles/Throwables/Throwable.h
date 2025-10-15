#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "Throwable.generated.h"

class UThrowableData;

UCLASS()
class POSSESSIONGAME_API AThrowable : public APoolObject
{
	GENERATED_BODY()
public:
	virtual void Throw(const float InImpulseForce);
	virtual void Disable(const bool bHide) override;
protected:
	AThrowable();
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UThrowableData> Data;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;

	virtual void OnImpact(const FHitResult& Hit);
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};

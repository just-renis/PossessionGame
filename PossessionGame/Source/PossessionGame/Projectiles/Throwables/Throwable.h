#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "Throwable.generated.h"

UCLASS()
class POSSESSIONGAME_API AThrowable : public APoolObject
{
	GENERATED_BODY()
public:
	virtual void Throw(const float InImpulseForce);
protected:
	AThrowable();
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
private:
	float CurrentScale = 1.f;
	UPROPERTY()
	FTimerHandle ScalingHandle;

	void ScalingProcess();
};

#pragma once

#include "CoreMinimal.h"
#include "Throwable.h"
#include "WraithMeteorTrigger.generated.h"

UCLASS()
class POSSESSIONGAME_API AWraithMeteorTrigger : public AThrowable
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MI_MeshBaseMaterial;

	virtual void BeginPlay() override;
	virtual void OnImpact(const FHitResult& InHit) override;
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};

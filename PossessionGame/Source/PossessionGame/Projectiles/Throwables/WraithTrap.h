#pragma once

#include "CoreMinimal.h"
#include "Throwable.h"
#include "WraithTrap.generated.h"

class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API AWraithTrap : public AThrowable
{
	GENERATED_BODY()
public:
	virtual void Init(const bool bInIsPlayerObject) override;
private:
	AWraithTrap();

	UPROPERTY()
	TObjectPtr<UAudioComponent> DeploySfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> TrailVfx;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MI_MeshBaseMaterial;

	virtual void BeginPlay() override;
	virtual void OnImpact(const FHitResult& InHit) override;
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};

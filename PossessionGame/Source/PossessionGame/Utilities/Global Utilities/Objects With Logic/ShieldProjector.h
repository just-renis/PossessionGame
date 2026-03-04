#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Destructibles/Destructible.h"
#include "ShieldProjector.generated.h"

UCLASS()
class POSSESSIONGAME_API AShieldProjector : public ADestructible
{
	GENERATED_BODY()

	AShieldProjector();

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float Duration = 10.f;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> ProjectorMesh;

	virtual void BeginPlay() override;
	virtual void Init(const bool bInIsPlayerObject) override;
	virtual void DestroyThis() override;
	void ChangeProjectorCollisionState(const bool bState) const;
	virtual void SetThisLifeSpan() override;
	virtual void SetUpgradesApplication() override;
	UFUNCTION()
	void OnProjectorLanded(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};

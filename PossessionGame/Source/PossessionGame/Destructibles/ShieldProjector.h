#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Destructibles/Destructible.h"
#include "ShieldProjector.generated.h"

enum class ESound : uint16;
class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API AShieldProjector : public ADestructible
{
	GENERATED_BODY()

	AShieldProjector();

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float Duration = 10.f;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> ProjectorMesh;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ConnectionVfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> DeploySfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> DissolveSfx;
	UPROPERTY()
	TMap<ESound, TObjectPtr<UAudioComponent>> Sounds;

	virtual void BeginPlay() override;
	virtual void Init(const bool bInIsPlayerObject) override;
	virtual void DestroyThis() override;
	void ChangeProjectorCollisionState(const bool bState) const;
	virtual void SetThisLifeSpan() override;
	virtual void SetUpgradesApplication() override;
	virtual void Disable(const bool bHide) override;
	void PlaySound(const ESound InSoundName);
	UFUNCTION()
	void OnProjectorLanded(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};

#include "VectorBlastProjectile.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/ESound.h"

AVectorBlastProjectile::AVectorBlastProjectile()
{
	HitSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("HitSfx"));
	HitSfx->SetAutoActivate(false);
	HitSfx->SetupAttachment(Mesh);
	BounceSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("BounceSfx"));
	BounceSfx->SetAutoActivate(false);
	BounceSfx->SetupAttachment(Mesh);
	Sounds.Add(ESound::VectorBlastHit, HitSfx);
	Sounds.Add(ESound::VectorBlastBounce, BounceSfx);
}

void AVectorBlastProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == this || OtherActor == nullptr) return;
	SpawnFirstProjectileImpactVfx(Hit);
/*	if (ProjectileData.bProjectileDealsAreaDamage)
	{
		DealAreaDamage();
		return;
	}*/
	if (OtherActor->ActorHasTag("Destructible"))
	{
		PlaySound(ESound::VectorBlastHit);
		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, (Hit.Location - GetActorLocation()).GetSafeNormal() * 2.f, Hit, ProjectileData.GetProprietor() ? ProjectileData.GetProprietor()->GetController() : nullptr, this, UDamageType::StaticClass());
		return;
	}
	if (!OtherActor->ActorHasTag("Enemy"))
	{
		PlaySound(ESound::VectorBlastBounce);
		return;
	}
	if (Mesh->GetCollisionObjectType() == ECC_GameTraceChannel1)
	{
		PlaySound(ESound::VectorBlastHit);
		DealDamageToSentientActor(true, OtherActor, Hit);
	}
}

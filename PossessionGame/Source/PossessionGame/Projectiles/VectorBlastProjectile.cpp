#include "VectorBlastProjectile.h"

#include "Kismet/GameplayStatics.h"

void AVectorBlastProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == this || OtherActor == nullptr) return;
//	SpawnFirstProjectileImpactVfx(Hit);
//	PlayImpactSound();
/*	if (ProjectileData.bProjectileDealsAreaDamage)
	{
		DealAreaDamage();
		return;
	}*/
	if (OtherActor->ActorHasTag("Destructible"))
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, (Hit.Location - GetActorLocation()).GetSafeNormal() * 2.f, Hit, ProjectileData.GetProprietor() ? ProjectileData.GetProprietor()->GetController() : nullptr, this, UDamageType::StaticClass());
		return;
	}
	if (!OtherActor->ActorHasTag("Enemy")) return;
	if (Mesh->GetCollisionObjectType() == ECC_GameTraceChannel1) DealDamageToSentientActor(true, OtherActor, Hit);
}

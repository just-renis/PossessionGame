#include "Throwable.h"

void AThrowable::Throw(const float InImpulseForce)
{
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetSimulatePhysics(true);
	Mesh->AddImpulse((-GetActorRightVector() + GetActorForwardVector() * 0.3f).GetSafeNormal() * InImpulseForce, NAME_None, true);
	Mesh->OnComponentHit.AddDynamic(this, &AThrowable::OnHit);
}

AThrowable::AThrowable()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;
}

void AThrowable::OnImpact(const FHitResult& Hit)
{
	Mesh->OnComponentHit.RemoveAll(this);
	SetActorLocation(Hit.ImpactNormal, true);
	SetActorRotation(FRotator::ZeroRotator);
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AThrowable::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (FMath::IsNearlyEqual(Hit.ImpactNormal.Z, 1.f, 0.1f)) OnImpact(Hit);
}

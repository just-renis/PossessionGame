#include "Throwable.h"
#include "PossessionGame/Data Assets/Base Data Assets/ThrowableData.h"

void AThrowable::Initialize(const TObjectPtr<AActor>& InProprietor) { Proprietor = InProprietor; }

void AThrowable::Throw()
{
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetSimulatePhysics(true); 
	Mesh->SetPhysicsLinearVelocity(FVector::ZeroVector, false);
	Mesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	Mesh->AddImpulse((-GetActorRightVector() + GetActorForwardVector() * 0.3f).GetSafeNormal() * Data->GetImpulseForce(), NAME_None, true);
	if (Data->GetNeedsOnHitEvent()) Mesh->OnComponentHit.AddDynamic(this, &AThrowable::OnHit);
}

void AThrowable::Disable(const bool bHide)
{
	if (Data->GetNeedsOnHitEvent()) Mesh->OnComponentHit.RemoveAll(this);
	Super::Disable(bHide);
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

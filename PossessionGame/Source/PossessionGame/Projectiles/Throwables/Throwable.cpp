#include "Throwable.h"

void AThrowable::Throw(const float InImpulseForce)
{
	Mesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Mesh->SetRelativeScale3D(FVector(4.f));
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetSimulatePhysics(true);
	Mesh->AddImpulse(-GetActorRightVector() * InImpulseForce, NAME_None, true);
	Mesh->OnComponentHit.AddDynamic(this, &AThrowable::OnHit);
}

AThrowable::AThrowable()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;
}

void AThrowable::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Error, TEXT("Normal: %s, Impact Normal: %s"), *Hit.Normal.ToString(), *Hit.ImpactNormal.ToString());
	if (FMath::IsNearlyEqual(Hit.ImpactNormal.Z, 1.f, 0.1f))
	{
		Mesh->OnComponentHit.RemoveAll(this);
		SetActorLocation(Hit.ImpactPoint, true);
		Mesh->SetSimulatePhysics(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

#include "Throwable.h"

void AThrowable::Throw(const FVector& InOwnerDirection, const float InImpulseForce)
{
	UE_LOG(LogTemp, Error, TEXT("InOwnerDIrection: %s"), *InOwnerDirection.ToString());
	Mesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Mesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
	GetWorldTimerManager().SetTimer(ScalingHandle, this, &AThrowable::ScalingProcess, 0.05f, true);
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

void AThrowable::ScalingProcess()
{
	CurrentScale += 0.8f;
	Mesh->SetRelativeScale3D(FVector(CurrentScale));
	if (CurrentScale >= 8.f) GetWorldTimerManager().ClearTimer(ScalingHandle);
}

void AThrowable::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Error, TEXT("Normal: %s, Impact Normal: %s"), *Hit.Normal.ToString(), *Hit.ImpactNormal.ToString());
	if (FMath::IsNearlyEqual(Hit.ImpactNormal.Z, 1.f, 0.1f))
	{
		Mesh->OnComponentHit.RemoveAll(this);
		SetActorLocation(Hit.ImpactNormal, true);
		Mesh->SetSimulatePhysics(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

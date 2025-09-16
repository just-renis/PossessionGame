#include "Throwable.h"

void AThrowable::Throw(const float InImpulseForce)
{
	GetWorldTimerManager().SetTimer(ScalingHandle, this, &AThrowable::ScalingProcess, 0.05f, true);
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

void AThrowable::ScalingProcess()
{
	CurrentScale += 0.7f;
	Mesh->SetRelativeScale3D(FVector(CurrentScale));
	if (CurrentScale >= 7.9f) GetWorldTimerManager().ClearTimer(ScalingHandle);
}

void AThrowable::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(ScalingHandle);
	Super::EndPlay(EndPlayReason);
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

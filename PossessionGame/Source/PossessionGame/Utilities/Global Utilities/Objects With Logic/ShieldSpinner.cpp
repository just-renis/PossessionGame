#include "ShieldSpinner.h"

AShieldSpinner::AShieldSpinner()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	InnerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InnerMesh"));
	OuterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OuterMesh"));
	InnerFloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InnerFloorMesh"));
	OuterFloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OuterFloorMesh"));
	InnerMesh->SetupAttachment(RootComponent);
	OuterMesh->SetupAttachment(RootComponent);
	InnerFloorMesh->SetupAttachment(RootComponent);
	OuterFloorMesh->SetupAttachment(RootComponent);
}

void AShieldSpinner::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(SpinHandle, this, &AShieldSpinner::ProcessSpin, 1.f / 60.f, true);
	SetupLiftInitiationTimer();
}

void AShieldSpinner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(SpinHandle);
	GetWorldTimerManager().ClearTimer(LiftingHandle);
	Super::EndPlay(EndPlayReason);
}

void AShieldSpinner::ProcessSpin() const
{
	InnerMesh->AddLocalRotation(FRotator(0.f, -RotationSpeed, 0.f));
	OuterMesh->AddLocalRotation(FRotator(0.f, RotationSpeed, 0.f));
	InnerFloorMesh->AddLocalRotation(FRotator(0.f, -RotationSpeed, 0.f));
	OuterFloorMesh->AddLocalRotation(FRotator(0.f, RotationSpeed, 0.f));
}

void AShieldSpinner::SetupLiftInitiationTimer()
{
	FTimerHandle LiftChangeHandle;
	GetWorldTimerManager().SetTimer(LiftChangeHandle, this, &AShieldSpinner::SetupLifting, SecondsBetweenLiftChange); 
}

void AShieldSpinner::SetupLifting()
{
	GetWorldTimerManager().SetTimer(LiftingHandle, this, &AShieldSpinner::ProcessLifting, 1.f / 60.f, true);
}

void AShieldSpinner::ProcessLifting()
{
	CurrentlyLiftedHeight += LiftHeightStep;
	const FVector Direction = bIsLiftingInner ? FVector::UpVector * LiftHeightStep : -FVector::UpVector * LiftHeightStep;
	InnerMesh->AddLocalOffset(Direction);
	OuterMesh->AddLocalOffset(-Direction);
	if (CurrentlyLiftedHeight >= LiftHeight)
	{
		bIsLiftingInner = !bIsLiftingInner;
		CurrentlyLiftedHeight = 0.f;
		GetWorldTimerManager().ClearTimer(LiftingHandle);
		SetupLiftInitiationTimer();
	}
}

#include "BreakableDoor.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

ABreakableDoor::ABreakableDoor()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Collision = CreateDefaultSubobject<USphereComponent>("Collision");
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = Mesh;
	Collision->SetupAttachment(Mesh);
}

void ABreakableDoor::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->MeleeAttackOccured.BindUObject(this, &ABreakableDoor::TryBreakingDoor);
	GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->EnableMeleeChecking.BindUObject(this, &ABreakableDoor::EnableCollision);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ABreakableDoor::EnableBreakable);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ABreakableDoor::DisableBreakable);
}

void ABreakableDoor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(PhysicsStopCheckHandle);
	GetWorldTimerManager().ClearTimer(DespawnProgressHandle);
	GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->MeleeAttackOccured.Unbind();
	GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->EnableMeleeChecking.Unbind();
	Super::EndPlay(EndPlayReason);
}

void ABreakableDoor::EnableCollision() const { Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly); }

void ABreakableDoor::TryBreakingDoor()
{
	if (!bIsBreakable) return;
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::DoorBreak);
	GetWorldTimerManager().SetTimer(PhysicsStopCheckHandle, this, &ABreakableDoor::CheckIfStoppedPhysics, 0.1f, true);
	MID = Mesh->CreateAndSetMaterialInstanceDynamic(0);
	GetWorldTimerManager().SetTimer(DespawnProgressHandle, this, &ABreakableDoor::SetupDespawnProgress, 5.f, false);
	bIsBreakable = true;
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(BreakDoorCameraShake);
	Mesh->SetSimulatePhysics(true);
	const FVector PlayerLocation = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();
	FVector Direction = GetActorLocation() - PlayerLocation;
	Direction.Z = 0.f;
	Direction = Direction.GetSafeNormal();
	Mesh->AddImpulse(Direction * Force * 100.f, NAME_None, true);
}

void ABreakableDoor::ProcessDespawnProgress()
{
	if (!MID) return;
	MID->SetScalarParameterValue("Progress", 0.4f + 0.03f * ++DespawnProgressIteration);
	if (DespawnProgressIteration == 10) Destroy();
}

void ABreakableDoor::SetupDespawnProgress()
{
	GetWorldTimerManager().SetTimer(DespawnProgressHandle, this, &ABreakableDoor::ProcessDespawnProgress, 0.075f, true);
}

void ABreakableDoor::CheckIfStoppedPhysics()
{
	if (Mesh->GetPhysicsLinearVelocity().Size() < 5.f)
	{
		Mesh->SetSimulatePhysics(false);
		GetWorldTimerManager().ClearTimer(PhysicsStopCheckHandle);
	}
}

void ABreakableDoor::EnableBreakable(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player")) return;
	bIsBreakable = true;
}

void ABreakableDoor::DisableBreakable(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor->ActorHasTag("Player")) return;
	bIsBreakable = false;
}

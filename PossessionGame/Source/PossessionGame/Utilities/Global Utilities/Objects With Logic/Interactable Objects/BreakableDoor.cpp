#include "BreakableDoor.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

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

void ABreakableDoor::EnableCollision() const
{
	if (!Collision) return;
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ABreakableDoor::TryBreakingDoor() const
{
	if (!bIsBreakable) return;
	Mesh->SetSimulatePhysics(true);
	const FVector PlayerLocation = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();
	FVector Direction = GetActorLocation() - PlayerLocation;
	Direction.Z = 0.f;
	Direction = Direction.GetSafeNormal();
	Mesh->AddImpulse(Direction * Force * 100.f, NAME_None, true);
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

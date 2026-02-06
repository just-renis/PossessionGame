#include "PyroBossFireFloor.h"
#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

APyroBossFireFloor::APyroBossFireFloor()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetCollisionProfileName(FName("TriggerPlayer"));
	Collision->OnComponentBeginOverlap.AddDynamic(this, &APyroBossFireFloor::AddOverlappedTarget);
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Collision->SetupAttachment(RootComponent);
}

void APyroBossFireFloor::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->TriggerFireFloors.AddUObject(this, &APyroBossFireFloor::TriggerFireWarning);
}

void APyroBossFireFloor::TriggerFireWarning(const TArray<int32>& InIds)
{
	if (!InIds.Contains(Id)) return;
	bIsWarningActive = true;
	if (!WarningFire) GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::PyroBossFloorFireWarning, [this]() -> bool { return bIsWarningActive; }, &WarningFire, Mesh);
	else WarningFire->Activate(true);
	GetWorldTimerManager().SetTimer(TriggerFireHandle, this, &APyroBossFireFloor::TriggerFire, TriggerFireTime);
}

void APyroBossFireFloor::TriggerFire()
{
	if (WarningFire) WarningFire->DeactivateImmediate();
	bIsWarningActive = false;
	if (Collision) Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	bIsFireActive = true;
	if (!EnvironmentFire) GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::PyroBossEnvironmentFire, [this]() -> bool { return bIsFireActive; }, &EnvironmentFire, Mesh);
	else EnvironmentFire->Activate(true);
	GetWorldTimerManager().SetTimer(FireUptimeHandle, this, &APyroBossFireFloor::TurnOffFire, FireUptime);
}

void APyroBossFireFloor::TurnOffFire()
{
	if (EnvironmentFire) EnvironmentFire->DeactivateImmediate();
	if (Collision) Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bIsFireActive = false;
}

void APyroBossFireFloor::AddOverlappedTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player")) return;
	Cast<AEnemy>(OtherActor)->SetupBurningState(EBurningType::PyroBossEnvironmentFire, 3.f, 4.f, 0.25f);
	if (Collision) Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

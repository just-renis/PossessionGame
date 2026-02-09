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
	TWeakObjectPtr WeakThis = this;
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->LoadVfx(EVfx::PyroBossFloorFireWarning, [WeakThis]
	{
		if (!WeakThis.IsValid() || !WeakThis.Get()) return;
		if (WeakThis->WarningFire)
		{
			WeakThis->WarningFire->DestroyComponent();
			WeakThis->WarningFire = nullptr;
		}
		WeakThis->GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::PyroBossFloorFireWarning, nullptr, &WeakThis->WarningFire, WeakThis->Mesh, NAME_None, true);
	});
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->LoadVfx(EVfx::PyroBossEnvironmentFire, [WeakThis]
	{
		if (!WeakThis.IsValid() || !WeakThis.Get()) return;
		if (WeakThis->EnvironmentFire)
		{
			WeakThis->EnvironmentFire->DestroyComponent();
			WeakThis->EnvironmentFire = nullptr;
		}
		WeakThis->GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::PyroBossEnvironmentFire, nullptr, &WeakThis->EnvironmentFire, WeakThis->Mesh, NAME_None, true);
	});
}

void APyroBossFireFloor::TriggerFireWarning(const TArray<int32>& InIds)
{
	if (!InIds.Contains(Id)) return;
	bIsWarningActive = true;
	if (WarningFire) WarningFire->Activate(true);
	GetWorldTimerManager().SetTimer(TriggerFireHandle, this, &APyroBossFireFloor::TriggerFire, TriggerFireTime);
}

void APyroBossFireFloor::TriggerFire()
{
	if (WarningFire) WarningFire->DeactivateImmediate();
	bIsWarningActive = false;
	if (Collision) Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	bIsFireActive = true;
	if (EnvironmentFire) EnvironmentFire->Activate(true);
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

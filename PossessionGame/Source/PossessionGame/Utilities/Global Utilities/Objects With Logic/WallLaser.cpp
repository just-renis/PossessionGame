#include "WallLaser.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

AWallLaser::AWallLaser()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	DamageRadius = CreateDefaultSubobject<UCapsuleComponent>(FName("DamageRadius"));
	DamageRadius->SetupAttachment(RootComponent);
	DamageRadius->SetCollisionProfileName("TriggerPlayer");
	DamageRadius->SetGenerateOverlapEvents(false);
	DamageRadius->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DamageRadius->OnComponentBeginOverlap.AddDynamic(this, &AWallLaser::AddOverlappedTarget);
	DamageRadius->OnComponentEndOverlap.AddDynamic(this, &AWallLaser::RemoveOverlappedTarget);
}

void AWallLaser::BeginPlay()
{
	Super::BeginPlay();
	MID_Material = Mesh->CreateAndSetMaterialInstanceDynamic(0);
	if (MID_Material) MID_Material->SetScalarParameterValue(FName("Appear"), 0.f);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->TriggerWallLasers.AddUObject(this, &AWallLaser::EnableLaser);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnElevatorTriggered.AddUObject(this, &AWallLaser::DisableLaser);
}

void AWallLaser::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnElevatorTriggered.RemoveAll(this);
	GetWorldTimerManager().ClearTimer(DamageOvertimeHandle);
	Super::EndPlay(EndPlayReason);
}

void AWallLaser::EnableLaser(const TArray<int32>& InIds, const int32 InGroup)
{
	if (Group != InGroup || !InIds.Contains(Id)) return;
	bIsLaserEnabled = true;
	if (MID_Material) MID_Material->SetScalarParameterValue(FName("Appear"), 100.f);
	DamageRadius->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DamageRadius->SetGenerateOverlapEvents(true);
	GetWorldTimerManager().SetTimer(DamageOvertimeHandle, this, &AWallLaser::DealDamage, DamageTick, true);
}

void AWallLaser::DisableLaser()
{
	if (!bIsLaserEnabled) return;
	if (MID_Material) MID_Material->SetScalarParameterValue(FName("Appear"), 0.f);
	DamageRadius->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DamageRadius->SetGenerateOverlapEvents(false);
	GetWorldTimerManager().ClearTimer(DamageOvertimeHandle);
}

void AWallLaser::DealDamage()
{
	if (!Target) return;
	UGameplayStatics::ApplyDamage(Target, Damage, nullptr, this, UDamageType::StaticClass());
}

void AWallLaser::AddTarget(const TObjectPtr<AEnemy>& InTarget) { Target = InTarget; }

void AWallLaser::RemoveTarget() { Target = nullptr; }

void AWallLaser::AddOverlappedTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag(FName("Player"))) return;
	if (const TObjectPtr<AEnemy> Player = Cast<AEnemy>(OtherActor)) AddTarget(Player);
}

void AWallLaser::RemoveOverlappedTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor->ActorHasTag(FName("Player"))) return;
	RemoveTarget();
}

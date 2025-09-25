#include "Warden.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/EStateTreeEvent.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"

AWarden::AWarden()
{
	if (AI_HpBarComponent)
	{
		AI_HpBarComponent->DestroyComponent();
		AI_HpBarComponent = nullptr;
	}
	BeamWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BeamWeapon"));
	BeamCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BeamCollision"));
	BeamCollision->SetupAttachment(BeamWeapon, FName("BeamSocket"));
	BeamCollision->SetCollisionProfileName(FName("TriggerPlayer"));
	BeamCollision->SetGenerateOverlapEvents(true);
	BeamCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DeathSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("DeathSound"));
	DeathSfx->SetAutoActivate(false);
	DeathSfx->SetupAttachment(GetMesh());
	SpawnSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("SpawnSound"));
	SpawnSfx->SetAutoActivate(false);
	SpawnSfx->SetupAttachment(GetMesh());
	BeamFireSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("BeamFireSound"));
	BeamFireSfx->SetAutoActivate(false);
	BeamFireSfx->SetupAttachment(GetMesh());
	Sounds.Add(ESound::WardenDeath, DeathSfx);
	Sounds.Add(ESound::WardenSpawn, SpawnSfx);
	Sounds.Add(ESound::WardenBeamFire, BeamFireSfx);
}

void AWarden::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::WardenBlasterWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void AWarden::AI_PlayDeathSound()
{
	Super::AI_PlayDeathSound();
	PlaySound(ESound::WardenDeath);
}

void AWarden::AI_PlaySpawnSound() { PlaySound(ESound::WardenSpawn, true); }

void AWarden::AI_PlayDeathElectrification()
{
	AssetsManager->AttachVfx(EVfx::WardenDeathElectrification, nullptr, nullptr, GetMesh(), "DeathSocket");
}

void AWarden::BeginPlay()
{
	CharacterName = ECharacterType::Warden;
	Super::BeginPlay();
	bCanShoot = true;
	(void)DelegateManager->OnBossSpawn.ExecuteIfBound(FLinearColor::Red);
	BeamWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("BeamWeaponSocket"));
	AssetsManager->AttachVfx(EVfx::WardenBeamShot, [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed(); }, &BeamVfx, BeamWeapon, FName("BeamSocket"), true);
	GetWorldTimerManager().SetTimer(BeamTriggerHandle, this, &AWarden::TryTriggeringBeamShot, 1.f, true);
	BeamCollision->OnComponentBeginOverlap.AddDynamic(this, &AWarden::OnBeamCollisionBeginOverlap);
}

void AWarden::SetEmpEffect(const float InEffectDuration)
{
	switch (GetAI()->GetCurrentStateTreeEvent())
	{
		case EStateTreeEvent::WardenBeamShot: AI_StopAbility(0); break;
		default: ;
	}
	Super::SetEmpEffect(InEffectDuration);
}

void AWarden::TryTriggeringBeamShot()
{
	if (!CanAbilityBeExecuted(0) || GetIsAbilityInAction() || !AI) return;
	AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::WardenBeamShot);
}

void AWarden::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(BeamTriggerHandle);
	Super::EndPlay(EndPlayReason);
}

void AWarden::DisableBeamCollision()
{
	if (BeamCollision) BeamCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetWorldTimerManager().ClearTimer(BeamDisableCollisionHandle);
}

void AWarden::InitiateDeath()
{
	Super::InitiateDeath();
	DelegateManager->OnBossDeath.Broadcast();
}

void AWarden::ReceiveDamage(const float InDamage, const EDamageReason InDamageReason)
{
	Super::ReceiveDamage(InDamage, InDamageReason);
	(void)DelegateManager->OnBossDamaged.ExecuteIfBound(CurrentHp, MaxHp);
}

void AWarden::TriggerBeam()
{
	if (!BeamVfx || !BeamCollision) return;
	const TObjectPtr<AActor> Player = AI->GetStateTreeData().GetTarget();
	if (!Player) return;
	PlaySound(ESound::WardenBeamFire);
	// Beam calculation logic.
	const FVector Start = BeamWeapon->GetSocketLocation(FName("BeamSocket"));
	const FVector Direction = Player->GetActorLocation() - Start;
	float Distance = Direction.Size();
	const FVector BeamForward = BeamWeapon->GetSocketRotation(FName("BeamSocket")).Vector();
	// Beam blocking logic.
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	const FVector LaserEnd = Start + BeamForward * 4000.f;
	FHitResult LaserHit;
	GetWorld()->LineTraceSingleByChannel(LaserHit, Start, LaserEnd, ECC_Visibility, QueryParams);
	if (LaserHit.bBlockingHit) Distance = LaserHit.Distance;
	// Beam collision setup.
	const FVector BeamCenter = Start + BeamForward * (Distance * 0.5f);
	const FRotator BeamRotation = BeamForward.Rotation() + FRotator(90.f, 0.f, 0.f);
	// Set capsule collision transform
	BeamCollision->SetWorldLocation(BeamCenter);
	BeamCollision->SetWorldRotation(BeamRotation);
	BeamCollision->SetCapsuleSize(35.f, Distance * 0.5f);
	// Beam length vfx change.
	BeamVfx->SetVectorParameter(FName("Scale Factor 1"), FVector(Distance * DistanceMulti, 1.f, 1.f));
	BeamVfx->SetVectorParameter(FName("Scale Factor001"), FVector(Distance * DistanceMulti, 1.f, 1.f));
	BeamVfx->Activate(true);
}

void AWarden::TriggerBeamDamage()
{
	BeamCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetWorldTimerManager().SetTimer(BeamDisableCollisionHandle, this, &AWarden::DisableBeamCollision, DamageTimeSpan);
}

void AWarden::OnBeamCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const TObjectPtr<AEnemy> Player = Cast<AEnemy>(OtherActor); Player->GetIsHijacked())
	{
		Player->ReceiveDamage(Damage, EDamageReason::None);
		DisableBeamCollision();
	}
}

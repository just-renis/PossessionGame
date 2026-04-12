#include "Warden.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/EStateTreeEvent.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"

AWarden::AWarden()
{
	AI_DestroyBossUnnecessaryComponents();
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

void AWarden::AI_ReceiveStunEffect(const bool bHitByOtherAI, const float InEffectDuration, const float InDamage, FVector InHitDirection)
{
	if (bHitByOtherAI) return;
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->ShowDamageNumber.ExecuteIfBound(GetDamageNumberColor(), InDamage, GetActorLocation(), GetActorForwardVector());
	ReceiveDamage(InDamage, EDamageReason::BreacherBashDamage, nullptr, nullptr);
}

void AWarden::AI_DisableActions(const float InEffectDuration, const EDisablingEffect InEffectName)
{
	Super::AI_DisableActions(InEffectDuration, InEffectName);
	if (BeamVfx && BeamVfx->IsActive()) BeamVfx->DeactivateImmediate(); 
}

void AWarden::BeginPlay()
{
	CharacterName = ECharacterType::Warden;
	bIsBoss = true;
	Super::BeginPlay();
	bCanShoot = true;
	DelegateManager->OnBossDeath.AddUObject(this, &AWarden::UpdateGlobalHpBarIfMultipleWardensExist);
	BeamWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("BeamWeaponSocket"));
	GetWorldTimerManager().SetTimer(BeamTriggerHandle, this, &AWarden::TryTriggeringBeamShot, 1.f, true);
	BeamCollision->OnComponentBeginOverlap.AddDynamic(this, &AWarden::OnBeamCollisionBeginOverlap);
}

void AWarden::TryTriggeringBeamShot()
{
	if (!CanAbilityBeExecuted(0) || GetIsAbilityInAction() || !AI || !AI->GetStateTreeData().GetStateTreeRef()) return;
	if (!BeamVfx) AssetsManager->AttachVfx(EVfx::WardenBeamShot, [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed(); }, &BeamVfx, BeamWeapon, FName("BeamSocket"), true);
	AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::WardenBeamShot);
}

void AWarden::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(BeamDisableCollisionHandle);
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
	GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::Usb, GetActorLocation())->Init();
	Super::InitiateDeath();
	DelegateManager->OnBossDeath.RemoveAll(this);
	DelegateManager->OnBossDeath.Broadcast();
}

void AWarden::ReceiveDamage(const float InDamage, const EDamageReason InDamageReason, const FPointDamageEvent* PointDamageEvent, const FRadialDamageEvent* RadialDamageEvent)
{
	Super::ReceiveDamage(InDamage, InDamageReason, PointDamageEvent, RadialDamageEvent);
	if (CurrentHp > 0.f) (void)DelegateManager->UpdateGlobalHpBar.ExecuteIfBound(CurrentHp, MaxHp);
}

void AWarden::ClearTimers()
{
	Super::ClearTimers();
	GetWorldTimerManager().ClearTimer(BeamTriggerHandle);
	GetWorldTimerManager().ClearTimer(BeamDisableCollisionHandle);
}

void AWarden::ApplyEmpEffect(const float InEffectDuration) { Super::ApplyEmpEffect(InEffectDuration * 0.5f); }

void AWarden::UpdateGlobalHpBarIfMultipleWardensExist() const { (void)DelegateManager->UpdateGlobalHpBar.ExecuteIfBound(CurrentHp, MaxHp); }

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
	BeamVfx->SetVariableVec3(FName("Scale Factor 1"), FVector(Distance * DistanceMulti, 1.f, 1.f));
	BeamVfx->SetVariableVec3(FName("Scale Factor001"), FVector(Distance * DistanceMulti, 1.f, 1.f));
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
		Player->ReceiveDamage(Damage, EDamageReason::None, nullptr, nullptr);
		DisableBeamCollision();
	}
}

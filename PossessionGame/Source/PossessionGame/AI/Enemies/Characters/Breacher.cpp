#include "Breacher.h"
#include "MinionBoss.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Enums/EMoveMode.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Projectiles/Throwables/BreacherBarrel.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void ABreacher::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::BreacherWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void ABreacher::AI_TriggerDeathVoiceLine() { AssetsManager->PlaySound2D(ESound::BreacherOnHijack); }

void ABreacher::P_Setup()
{
	DelegateManager->OnWeaponAmmoChangeTwoParams.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	Super::P_Setup();
	Weapon->GetHandsAnimInstance()->OnHandsMontageEnd.BindUObject(this, &ABreacher::SetIsActionInMotion);
}

void ABreacher::P_StopBash()
{
	if (GetWeapon()->GetHandsAnimInstance()) GetWeapon()->GetHandsAnimInstance()->StopMontage(EHandsAnimState::BreacherBash, 0.f);
	SetIsActionInMotion(false);
	SetMaxWalkSpeed(EMoveMode::Walk);
}

void ABreacher::P_BashHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this) return;
	GetWorldTimerManager().ClearTimer(StopBashHandle);
	P_StopBash();
	AssetsManager->PlaySound2D(ESound::BreacherBashImpact);
	GetCapsuleComponent()->OnComponentHit.RemoveAll(this);
	BashCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BashCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABreacher::P_ThrowBarrel()
{
	const TObjectPtr<ABreacherBarrel> Barrel = Cast<ABreacherBarrel>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::BreacherBarrel, Weapon->GetHandsSocketLocation(FName("BarrelSocket")), Weapon->GetHandsSocketRotation(FName("BarrelSocket"))));
	if (!Barrel) return;
	Barrel->Initialize(this);
	Barrel->Init(true);
	AssetsManager->PlaySound2D(ESound::BreacherThrowBarrel);
	Barrel->Throw();
}

void ABreacher::P_Bash()
{
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ABreacher::P_BashHit);
	if (!BashVfx) AssetsManager->AttachVfx(EVfx::BreacherBash, [this]() -> bool { return GetIsAlive() && bIsHijacked; }, &BashVfx, GetMesh(), FName("DeathSocket"));
	else BashVfx->Activate();
	AssetsManager->PlaySound2D(ESound::BreacherBash);
	FVector DashDirection = FVector(GetVelocity().X, GetVelocity().Y, 0.f).GetSafeNormal();
	if (DashDirection.IsNearlyZero()) DashDirection = GetActorForwardVector();
	DashDirection.Z = 0.f;
	GetCharacterMovement()->StopMovementImmediately();
	LaunchCharacter(DashDirection * BashStrength, true, true);
	GetWorldTimerManager().SetTimer(StopBashHandle, this, &ABreacher::P_StopBash, 0.2f, false);
}

ABreacher::ABreacher()
{
	SpawnSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("SpawnSfx"));
	SpawnSfx->SetAutoActivate(false);
	SpawnSfx->SetupAttachment(GetMesh());
	DeathSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("DeathSfx"));
	DeathSfx->SetAutoActivate(false);
	DeathSfx->SetupAttachment(GetMesh());
	Sounds.Add(ESound::BreacherSpawn, SpawnSfx);
	Sounds.Add(ESound::BreacherDeath, DeathSfx);
	BashCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BashCollision"));
	BashCollision->SetupAttachment(GetMesh());
	BashCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABreacher::BeginPlay()
{
	CharacterName = ECharacterType::Breacher;
	Super::BeginPlay();
	BashCollision->OnComponentBeginOverlap.AddDynamic(this, &ABreacher::OnBashCollisionBeginOverlap);
}

void ABreacher::OnBashCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this || !OtherActor->ActorHasTag("AliveEnemy")) return;
	FVector HitDirection = (OtherActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	HitDirection.Z = 0.f; 
	Cast<AEnemy>(OtherActor)->ReceiveStunEffect(BashStunDuration, BashDamage, HitDirection);
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + HitDirection * 300.f, FColor::Red, false, 2.f, 0, 2.f);
}

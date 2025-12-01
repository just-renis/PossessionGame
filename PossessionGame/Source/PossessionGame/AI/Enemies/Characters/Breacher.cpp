#include "Breacher.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "MinionBoss.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EMoveMode.h"
#include "PossessionGame/Enums/EStateTreeEvent.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Projectiles/Throwables/BreacherBarrel.h"
#include "PossessionGame/Projectiles/Throwables/BreacherDynamite.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

float ABreacher::AI_GetShootPause() { return Super::AI_GetShootPause() * (bAI_ShootSpeedDoubled ? 0.5f : 1.f); }

void ABreacher::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::BreacherWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void ABreacher::AI_TriggerDeathVoiceLine() { AssetsManager->PlaySound2D(ESound::BreacherOnHijack); }

void ABreacher::AI_TryTriggeringBash()
{
	UE_LOG(LogTemp, Error, TEXT("AI_TryTriggeringBash 0"));
	if (!CanAbilityBeExecuted(1) || GetIsAbilityInAction() || !AI || !AI->GetStateTreeData().GetStateTreeRef()) return;
	UE_LOG(LogTemp, Error, TEXT("AI_TryTriggeringBash 1"));
	AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::BreacherBash);
	GetWorld()->GetTimerManager().ClearTimer(AI_TriggerBashHandle);
}

void ABreacher::P_Setup()
{
	GetWorldTimerManager().ClearTimer(AI_TriggerBashHandle);
	bImmuneToExplosions = false;
	DelegateManager->OnWeaponAmmoChangeTwoParams.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	Super::P_Setup();
	Weapon->GetHandsAnimInstance()->OnHandsMontageEnd.BindUObject(this, &ABreacher::SetIsActionInMotion);
}

void ABreacher::P_StopBash()
{
	GetCapsuleComponent()->OnComponentHit.RemoveAll(this);
	SetIsActionInMotion(false);
	SetMaxWalkSpeed(EMoveMode::Walk);
}

void ABreacher::P_SetupRightClickInput(const TObjectPtr<UEnhancedInputComponent> InEnhancedInputComponent)
{
	InEnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(14).Action, ETriggerEvent::Started, this, &ABreacher::DetonateDynamites);
}

void ABreacher::P_BashHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this || FMath::Abs(Hit.Normal.Z) > 0.5f) return;
	P_MakeCameraShake(BashCameraShake);
	GetWorldTimerManager().ClearTimer(StopBashHandle);
	P_StopBash();
	AssetsManager->PlaySound2D(ESound::BreacherBashImpact);
	BashCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BashCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABreacher::AI_EnableTriggeringBash()
{
	UE_LOG(LogTemp, Error, TEXT("AI_EnableTriggeringBash"));
	GetWorldTimerManager().SetTimer(AI_TriggerBashHandle, this, &ABreacher::AI_TryTriggeringBash, 1.f, true);
}

void ABreacher::P_ThrowBarrel()
{
	const TObjectPtr<ABreacherBarrel> Barrel = Cast<ABreacherBarrel>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectWithCapFromPool(EPoolObject::BreacherBarrel, Weapon->GetHandsSocketLocation(FName("BarrelSocket")), Weapon->GetHandsSocketRotation(FName("BarrelSocket"))));
	if (!Barrel) return;
	Barrel->Initialize(this);
	Barrel->Init(true);
	AssetsManager->PlaySound2D(ESound::BreacherThrowBarrel);
	Barrel->Throw();
}

void ABreacher::P_Bash(const float InImpulseStrength)
{
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ABreacher::P_BashHit);
	if (!BashVfx) AssetsManager->AttachVfx(EVfx::BreacherBash, [this]() -> bool { return GetIsAlive() && bIsHijacked; }, &BashVfx, GetMesh(), FName("DeathSocket"));
	else BashVfx->Activate();
	AssetsManager->PlaySound2D(ESound::BreacherBash);
	FVector DashDirection = GetActorForwardVector();
	DashDirection.Z = 0.f;
	DashDirection.Normalize();
	GetCharacterMovement()->StopMovementImmediately();
	LaunchCharacter(DashDirection * InImpulseStrength * 1000.f, true, true);
	GetWorldTimerManager().SetTimer(StopBashHandle, this, &ABreacher::P_StopBash, 0.2f, false);
}

void ABreacher::P_ThrowDynamite()
{
	const TObjectPtr<ABreacherDynamite> Dynamite = Cast<ABreacherDynamite>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectWithCapFromPool(EPoolObject::BreacherDynamite, Weapon->GetHandsSocketLocation(FName("BarrelSocket")), Weapon->GetHandsSocketRotation(FName("BarrelSocket"))));
	if (!Dynamite) return;
	Dynamite->Initialize(this);
	Dynamite->Init(true);
	AssetsManager->PlaySound2D(ESound::BreacherThrowBarrel);
	Dynamite->Throw();
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

float ABreacher::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bImmuneToExplosions && DamageEvent.IsOfType(FRadialDamageEvent::ClassID)) return 0.f;
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ABreacher::BeginPlay()
{
	CharacterName = ECharacterType::Breacher;
	Super::BeginPlay();
	bImmuneToExplosions = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->IsSpecialUpgradeAvailable(EUpgrade::BreacherAbility3, EUpgradeAttribute::Special1);
	bAI_ShootSpeedDoubled = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->IsSpecialUpgradeAvailable(EUpgrade::BreacherAbility1, EUpgradeAttribute::Special1);
	BashCollision->OnComponentBeginOverlap.AddDynamic(this, &ABreacher::OnBashCollisionBeginOverlap);
}

void ABreacher::DetonateDynamites() { DelegateManager->OnDetonateDynamites.Broadcast(); }

void ABreacher::InitiateDeath()
{
	GetWorld()->GetTimerManager().ClearTimer(AI_TriggerBashHandle);
	Super::InitiateDeath();
}

void ABreacher::OnBashCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this || !OtherActor->ActorHasTag("AliveEnemy")) return;
	FVector HitDirection = (OtherActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	HitDirection.Z = 0.f;
	float BashDamage = DefaultBashDamage;
	const TObjectPtr<URunSaveSlot> RunSaveSlot = GetWorld()->GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot();
	BashDamage += RunSaveSlot->GetUpgradeAttributeValues(EUpgrade::BreacherAbility2, EUpgradeAttribute::FlatDamage, true);
	Cast<AEnemy>(OtherActor)->ReceiveStunEffect(BashStunDuration, BashDamage, HitDirection);
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + HitDirection * 300.f, FColor::Red, false, 2.f, 0, 2.f);
}

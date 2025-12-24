#include "BreacherBarrel.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Data Assets/Base Data Assets/ThrowableData.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

void ABreacherBarrel::Explode()
{
	const TObjectPtr<URunSaveSlot> RunSaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot();
	const float RadiusIncrease = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::BreacherAbility1, EUpgradeAttribute::Size, true);
	const float DamageFromUpgrades = RunSaveSlot->GetUpgradeAttributeValues(EUpgrade::BreacherAbility1, EUpgradeAttribute::FlatDamage, true);
	const bool bIsTripleDamageChanceEnabled = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->IsSpecialUpgradeAvailable(EUpgrade::BreacherAbility1, EUpgradeAttribute::Special1);
	const bool bIsTripleDamageRolled = bIsTripleDamageChanceEnabled ? FMath::RandRange(1, 10) < 4 : false;
	float NormalDamage = (Data->GetDamage() + DamageFromUpgrades) * (bIsTripleDamageRolled ? 3.f : 1.f);
	float MinimumDamage = (Data->GetRadiusMinimumDamage() + DamageFromUpgrades) * (bIsTripleDamageRolled ? 3.f : 1.f);
	const float InnerRadius = Data->GetDamageInnerRadius() + Data->GetDamageInnerRadius() * RadiusIncrease * 0.01f;
	const float OuterRadius = Data->GetDamageOuterRadius() + Data->GetDamageOuterRadius() * RadiusIncrease * 0.01f;
	const float PlayerLevelGlobalDamageBoost = GetPlayerLevelAllGlobalDamageBonus();
	UE_LOG(LogTemp, Error, TEXT("BEFORE BARREL: %f"), NormalDamage);
	NormalDamage += PlayerLevelGlobalDamageBoost;
	MinimumDamage += PlayerLevelGlobalDamageBoost;
	UE_LOG(LogTemp, Error, TEXT("AFTER BARREL: %f"), NormalDamage);
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), NormalDamage, MinimumDamage, GetActorLocation(), InnerRadius, OuterRadius,
	Data->GetDamageFalloff(), UDamageType::StaticClass(), TArray<AActor*>(), this, nullptr, ECC_GameTraceChannel4);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->SpawnVfx(bIsTripleDamageRolled ? EVfx::BreacherSpecialBarrelExplosion : EVfx::BreacherBarrelExplosion, nullptr, GetActorLocation());
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestObjectWithCapRemoval.ExecuteIfBound(EPoolObject::BreacherBarrel, this);
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::BreacherBarrel, this);
	PlaySound(bIsTripleDamageRolled ? ESound::BreacherSpecialBarrelExplosion : ESound::BreacherBarrelExplosion);
}

void ABreacherBarrel::Disable(const bool bHide)
{
	Mesh->SetSimulatePhysics(false);
	Super::Disable(bHide);
}

void ABreacherBarrel::Init(const bool bInIsPlayerObject)
{
	bIsAlreadyExploding = false;
	Super::Init(bInIsPlayerObject);
}

ABreacherBarrel::ABreacherBarrel()
{
	ExplodeSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("ExplodeSfx"));
	ExplodeSfx->SetAutoActivate(false);
	ExplodeSfx->SetupAttachment(Mesh);
	SpecialExplodeSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("SpecialExplodeSfx"));
	SpecialExplodeSfx->SetAutoActivate(false);
	SpecialExplodeSfx->SetupAttachment(Mesh);
	Sounds.Add(ESound::BreacherBarrelExplosion, ExplodeSfx);
	Sounds.Add(ESound::BreacherSpecialBarrelExplosion, ExplodeSfx);
}

void ABreacherBarrel::BeginPlay()
{
	Super::BeginPlay();
	Tags.Add(FName("BreacherBarrel"));
}

float ABreacherBarrel::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsAlreadyExploding) return 0.f;
	bIsAlreadyExploding = true;
	Explode();
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

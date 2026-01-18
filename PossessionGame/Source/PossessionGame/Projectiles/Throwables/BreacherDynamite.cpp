#include "BreacherDynamite.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Data Assets/Base Data Assets/ThrowableData.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

ABreacherDynamite::ABreacherDynamite()
{
	ExplodeSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("ExplodeSfx"));
	ExplodeSfx->SetAutoActivate(false);
	ExplodeSfx->SetupAttachment(Mesh);
	StickSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("StickSfx"));
	StickSfx->SetAutoActivate(false);
	StickSfx->SetupAttachment(Mesh);
	Sounds.Add(ESound::BreacherDynamiteExplosion, ExplodeSfx);
	Sounds.Add(ESound::BreacherDynamiteStick, StickSfx);
}

void ABreacherDynamite::PrimaryExplode()
{
	bInExplosionProcess = true;
	SavedSpecialExplosionLocation = GetActorLocation();
	Explode();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnDetonateDynamites.RemoveAll(this);
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestObjectWithCapRemoval.ExecuteIfBound(EPoolObject::BreacherDynamite, this);
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::BreacherDynamite, this);
	bInExplosionProcess = false;
	if (GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->IsSpecialUpgradeAvailable(EUpgrade::BreacherAbility3, EUpgradeAttribute::Special1))
	{
		GetWorldTimerManager().SetTimer(SpecialExplosionDelayHandle, this, &ABreacherDynamite::Explode, SpecialExplosionDelay);
	}
}

void ABreacherDynamite::Explode()
{
	const TObjectPtr<URunSaveSlot> RunSaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot();
	const float RadiusIncrease = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::BreacherAbility3, EUpgradeAttribute::Size, true);
	const float DamageFromUpgrades = RunSaveSlot->GetUpgradeAttributeValues(EUpgrade::BreacherAbility3, EUpgradeAttribute::FlatDamage, true);
	float NormalDamage = Data->GetDamage() + DamageFromUpgrades;
	float MinimumDamage = Data->GetRadiusMinimumDamage() + DamageFromUpgrades;
	const float InnerRadius = Data->GetDamageInnerRadius() + Data->GetDamageInnerRadius() * RadiusIncrease * 0.01f;
	const float OuterRadius = Data->GetDamageOuterRadius() + Data->GetDamageOuterRadius() * RadiusIncrease * 0.01f;
	const float PlayerLevelGlobalDamageBoost = GetPlayerLevelAllGlobalDamageBonus();
	NormalDamage += PlayerLevelGlobalDamageBoost;
	MinimumDamage += PlayerLevelGlobalDamageBoost;
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), NormalDamage, MinimumDamage, SavedSpecialExplosionLocation, InnerRadius, OuterRadius,
	Data->GetDamageFalloff(), UDamageType::StaticClass(), TArray<AActor*>(), this, Proprietor->GetInstigatorController(), ECC_GameTraceChannel4);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->SpawnVfx(EVfx::BreacherDynamiteExplosion, nullptr, SavedSpecialExplosionLocation);
	PlaySound(ESound::BreacherDynamiteExplosion);
}

void ABreacherDynamite::Init(const bool bInIsPlayerObject)
{
	Super::Init(bInIsPlayerObject);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnDetonateDynamites.AddUObject(this, &ABreacherDynamite::PrimaryExplode);
}

void ABreacherDynamite::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) { OnImpact(Hit); }

void ABreacherDynamite::OnImpact(const FHitResult& Hit)
{
	Mesh->OnComponentHit.RemoveAll(this);
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->AttachToComponent(Hit.GetComponent(), FAttachmentTransformRules::KeepWorldTransform);
}

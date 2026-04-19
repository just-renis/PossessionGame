#include "WraithMeteorProjectile.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Data Assets/Base Data Assets/CodexProjectileData.h"
#include "PossessionGame/Enums/ECameraShake.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

AWraithMeteorProjectile::AWraithMeteorProjectile()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SpawnSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("SpawnSfx"));
	SpawnSfx->SetAutoActivate(false);
	SpawnSfx->SetupAttachment(RootComponent);
	Sounds.Add(ESound::WraithMeteorSpawn, SpawnSfx);
}

EDamageReason AWraithMeteorProjectile::GetDamageReason() const { return bIsHijacked ? EDamageReason::WraithPlayerMeteor : EDamageReason::WraithEnemyMeteor; }

void AWraithMeteorProjectile::InitWithProprietor(const TObjectPtr<AEnemy> InProprietor)
{
	Super::InitWithProprietor(InProprietor);
	Proprietor = InProprietor;
	if (!Proprietor) return;
	PlaySound(ESound::WraithMeteorSpawn);
	bIsHijacked = Proprietor->GetIsHijacked();
	if (bIsHijacked)
	{
		if (!PlayerDeployVfx) GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::WraithMeteor, nullptr, &PlayerDeployVfx, RootComponent);
		else PlayerDeployVfx->Activate(true);
	}
	else
	{
		if (!EnemyDeployVfx) GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::EnemyWraithMeteor, nullptr, &EnemyDeployVfx, RootComponent);
		else EnemyDeployVfx->Activate(true);
	}
	GetWorld()->GetTimerManager().SetTimer(MeteorLandHandle, this, &AWraithMeteorProjectile::Landed, MeteorLandTime);
	GetWorld()->GetTimerManager().SetTimer(LifeSpanFinishedHandle, this, &AWraithMeteorProjectile::ReturnToPool, DefaultLifeSpan);
}

void AWraithMeteorProjectile::ReturnToPool()
{
	if (bIsHijacked)
	{
		if (PlayerDeployVfx) PlayerDeployVfx->DeactivateImmediate();
	}
	else
	{
		if (EnemyDeployVfx) EnemyDeployVfx->DeactivateImmediate();
	}
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::WraithMeteorProjectile, this);
}

void AWraithMeteorProjectile::Landed()
{
	const TObjectPtr<UCodexProjectileData> Data = GetProjectileData();
	if (!Data || !Proprietor) return;
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestTriggerCameraShake.ExecuteIfBound(ECameraShake::WraithMeteorLandCameraShake);
	Damage = DefaultDamage = Data->GetDamage();
	const TObjectPtr<USaveSlotManager> SaveSlotManager = GetGameInstance()->GetSubsystem<USaveSlotManager>();
	const TObjectPtr<URunSaveSlot> RunSaveSlot = SaveSlotManager->GetRunSaveSlot();
	Damage += RunSaveSlot->GetUpgradeAttributeValues(EUpgrade::WraithAbility3, EUpgradeAttribute::FlatDamage, bIsHijacked);
	if (bIsHijacked) Damage += DefaultDamage * SaveSlotManager->GetSaveSlot()->GetPlayerLevelAllGlobalDamageBonus();
	else
	{
		const float DamagePercentage = RunSaveSlot->GetUpgradeAttributeValues(EUpgrade::WraithAbility3, EUpgradeAttribute::DamagePercentage, bIsHijacked);
		Damage += DefaultDamage * DamagePercentage * 0.01f;
	}
	TArray<AActor*> IgnoredActors;
	if (!Data->CanBeDamagedByOwnProjectile() && !bIsHijacked)
	{
		const TArray<TObjectPtr<AEnemy>>& Enemies = GetWorld()->GetSubsystem<UEnemyCommunicator>()->GetAllEnemies();
		IgnoredActors.Reserve(Enemies.Num());
		for (AEnemy* Enemy : Enemies) IgnoredActors.Add(Enemy);
	}
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), Damage, Damage, GetActorLocation(), Data->GetDamageInnerRadius(),
	Data->GetDamageOuterRadius(), 0.f, UDamageType::StaticClass(), IgnoredActors, this, Proprietor->GetController(), ECC_GameTraceChannel4);
#if WITH_EDITOR
	if (bEnableDebug) DrawDebugSphere(GetWorld(), GetActorLocation(), Data->GetDamageInnerRadius(), 32, FColor::Green, false, 3.f);
#endif
}

void AWraithMeteorProjectile::PlaySound(const ESound InSoundName)
{
	Sounds[InSoundName]->Sound ? Sounds[InSoundName]->Play() :
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(Sounds[InSoundName], TFunction<bool()>(), InSoundName);
}

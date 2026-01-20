#include "WraithMeteorProjectile.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Data Assets/Base Data Assets/CodexProjectileData.h"
#include "PossessionGame/Enums/ECameraShake.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

AWraithMeteorProjectile::AWraithMeteorProjectile()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SpawnSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("SpawnSfx"));
	SpawnSfx->SetAutoActivate(false);
	SpawnSfx->SetupAttachment(RootComponent);
	Sounds.Add(ESound::WraithMeteorSpawn, SpawnSfx);
}

void AWraithMeteorProjectile::Init(const TObjectPtr<AEnemy> InProprietor)
{
	Super::Init(InProprietor);
	Proprietor = InProprietor;
	if (!Proprietor) return;
	PlaySound(ESound::WraithMeteorSpawn);
	bIsHijacked = Proprietor->GetIsHijacked();
	if (!DeployVfx) GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::WraithMeteor, nullptr, &DeployVfx, RootComponent);
	else DeployVfx->Activate(true);
	GetWorld()->GetTimerManager().SetTimer(MeteorLandHandle, this, &AWraithMeteorProjectile::Landed, MeteorLandTime);
	GetWorld()->GetTimerManager().SetTimer(LifeSpanFinishedHandle, this, &AWraithMeteorProjectile::ReturnToPool, DefaultLifeSpan);
}

void AWraithMeteorProjectile::ReturnToPool()
{
	if (DeployVfx) DeployVfx->DeactivateImmediate();
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::WraithMeteorProjectile, this);
}

void AWraithMeteorProjectile::Landed()
{
	const TObjectPtr<UCodexProjectileData> Data = GetProjectileData();
	if (!Data || !Proprietor) return;
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestTriggerCameraShake.ExecuteIfBound(ECameraShake::WraithMeteorLandCameraShake);
	Damage = DefaultDamage = Data->GetDamage();
	TArray<TObjectPtr<AActor>> IgnoredActors;
	if (!Data->CanBeDamagedByOwnProjectile() && !bIsHijacked) IgnoredActors = GetWorld()->GetSubsystem<UEnemyCommunicator>()->GetAllEnemies();
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), Damage, Damage, GetActorLocation(), Data->GetDamageInnerRadius(),
	Data->GetDamageOuterRadius(), 0.f, UDamageType::StaticClass(), IgnoredActors, this, Proprietor->GetController(), ECC_GameTraceChannel4);
}

void AWraithMeteorProjectile::PlaySound(const ESound InSoundName)
{
	Sounds[InSoundName]->Sound ? Sounds[InSoundName]->Play() :
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(Sounds[InSoundName], TFunction<bool()>(), InSoundName);
}

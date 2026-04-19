#include "SoldierBossMissile.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/ECameraShake.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

ASoldierBossMissile::ASoldierBossMissile()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	ImpactSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("ImpactSfx"));
	ImpactSfx->SetAutoActivate(false);
	ImpactSfx->SetupAttachment(RootComponent);
}

void ASoldierBossMissile::InitWithProprietor(const TObjectPtr<AEnemy> InProprietor)
{
	Super::InitWithProprietor(InProprietor);
	Proprietor = InProprietor;
	FTimerHandle LandHandle;
	GetWorld()->GetTimerManager().SetTimer(LandHandle, this, &ASoldierBossMissile::Landed, LandTime);
}

void ASoldierBossMissile::Landed()
{
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestTriggerCameraShake.ExecuteIfBound(ECameraShake::SoldierBossMissileImpactCameraShake);
	if (ImpactSfx) ImpactSfx->Sound ? ImpactSfx->Play() : GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(ImpactSfx, TFunction<bool()>(), ESound::SoldierBossMissileLand, false);
#if WITH_EDITOR
	if (bEnableDebug) DrawDebugSphere(GetWorld(), GetActorLocation(), AreaInMeters * 100.f, 32, FColor::Green, false, 3.f);
#endif
	TArray<AActor*> IgnoredActors;
	const TArray<TObjectPtr<AEnemy>>& Enemies = GetWorld()->GetSubsystem<UEnemyCommunicator>()->GetAllEnemies();
	IgnoredActors.Reserve(Enemies.Num());
	for (AEnemy* Enemy : Enemies) IgnoredActors.Add(Enemy);
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), Damage, Damage, GetActorLocation(), AreaInMeters * 100.f,
	AreaInMeters * 100.f, 0.f, UDamageType::StaticClass(), IgnoredActors, this, Proprietor->GetController(), ECC_GameTraceChannel4);
	ReturnToPool();
}

void ASoldierBossMissile::ReturnToPool() { GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::SoldierBossMissile, this); }

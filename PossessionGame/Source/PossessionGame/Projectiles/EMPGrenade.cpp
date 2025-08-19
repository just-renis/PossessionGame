#include "EMPGrenade.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Utilities/Subsystems/UpgradeManager.h"

AEMPGrenade::AEMPGrenade()
{
	Radius = CreateDefaultSubobject<USphereComponent>("radius");
	Radius->SetupAttachment(RootComponent);
	Radius->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Radius->OnComponentBeginOverlap.AddDynamic(this, &AEMPGrenade::TriggerEmpEffect);
	Sounds.Add(ESound::TricksterEmpImpact, CreateDefaultSubobject<UAudioComponent>(TEXT("EmpImpactSound")));
	Sounds[ESound::TricksterEmpImpact]->SetAutoActivate(false);
	Sounds[ESound::TricksterEmpImpact]->SetupAttachment(Mesh);
}

void AEMPGrenade::PlayImpactSound() { PlaySound(ESound::TricksterEmpImpact); }

void AEMPGrenade::OnImpact(const FHitResult& Hit)
{
	Radius->SetGenerateOverlapEvents(true);
	Radius->UpdateOverlaps();
	Super::OnImpact(Hit);
}

void AEMPGrenade::Init(FProjectileData& InProjectileData, const FRotator& Rotation, const FVector& Scale, const int32 InWhich)
{
	Super::Init(InProjectileData, Rotation, Scale, InWhich);
	const float EffectDurationIncrease = 0.f; //GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(ProjectileData.bIsPlayerProjectile, EPoolObject::TricksterEmpProjectile, EUpgradeAttribute::EffectDuration);
	EffectDuration = ProjectileData.EffectDuration + ProjectileData.EffectDuration * EffectDurationIncrease * 0.01f;
}

void AEMPGrenade::Disable(const bool bHide)
{
	Super::Disable();
	Radius->SetGenerateOverlapEvents(false);
}

void AEMPGrenade::TriggerEmpEffect(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const TObjectPtr<AEnemy> Target = Cast<AEnemy>(OtherActor)) Target->SetEmpEffect(EffectDuration);
}

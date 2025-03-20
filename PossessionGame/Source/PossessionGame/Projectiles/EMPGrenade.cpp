#include "EMPGrenade.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/Enums/ESound.h"

AEMPGrenade::AEMPGrenade()
{
	Radius = CreateDefaultSubobject<USphereComponent>("radius");
	Radius->SetupAttachment(RootComponent);
	Radius->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Radius->OnComponentBeginOverlap.AddDynamic(this, &AEMPGrenade::TriggerEMPEffect);
	Sounds.Add(ESound::TricksterEMPImpact, CreateDefaultSubobject<UAudioComponent>(TEXT("EMPImpactSound")));
	Sounds[ESound::TricksterEMPImpact]->SetAutoActivate(false);
	Sounds[ESound::TricksterEMPImpact]->SetupAttachment(Mesh);
}

void AEMPGrenade::PlayImpactSound() { PlaySound(ESound::TricksterEMPImpact); }

void AEMPGrenade::OnImpact(const FHitResult& Hit)
{
	Radius->SetGenerateOverlapEvents(true);
	Radius->UpdateOverlaps();
	Super::OnImpact(Hit);
}

void AEMPGrenade::Disable(const bool bHide)
{
	Super::Disable();
	Radius->SetGenerateOverlapEvents(false);
}

void AEMPGrenade::TriggerEMPEffect(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const TObjectPtr<AEnemy> Target = Cast<AEnemy>(OtherActor)) Target->SetEMPEffect();
}

#include "EMPGrenade.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/Enums/ESound.h"

AEMPGrenade::AEMPGrenade()
{
	Radius = CreateDefaultSubobject<USphereComponent>("radius");
	Radius->SetupAttachment(RootComponent);
	Radius->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Sounds.Add(ESound::TricksterEMPImpact, CreateDefaultSubobject<UAudioComponent>(TEXT("EMPImpactSound")));
	Sounds[ESound::TricksterEMPImpact]->SetAutoActivate(false);
	Sounds[ESound::TricksterEMPImpact]->SetupAttachment(Mesh);
}

void AEMPGrenade::PlayImpactSound() { PlaySound(ESound::TricksterEMPImpact); }

void AEMPGrenade::OnImpact(const FHitResult& Hit)
{
	Super::OnImpact(Hit);
	Radius->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerEMPEffect();
}

void AEMPGrenade::TriggerEMPEffect() const
{
	TArray<AActor*> OverlappingActors;
	Radius->GetOverlappingActors(OverlappingActors);
	UE_LOG(LogTemp, Warning, TEXT("TRIGGGGGGGERED"));
	for (AActor* Actor : OverlappingActors)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACTOR NAME: %s"), *GetName());
		if (const TObjectPtr<AEnemy> Target = Cast<AEnemy>(Actor)) Target->SetEMPEffect();
	}
	Radius->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

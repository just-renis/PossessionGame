#include "RangerGrenade.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Enums/ESound.h"

ARangerGrenade::ARangerGrenade()
{
	Sounds.Add(ESound::RangerGrenadeExplosion, CreateDefaultSubobject<UAudioComponent>(TEXT("grenadeExplosionSound")));
	Sounds[ESound::RangerGrenadeExplosion]->SetAutoActivate(false);
}

void ARangerGrenade::LoadSounds()
{
	Sounds[ESound::RangerGrenadeExplosion]->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void ARangerGrenade::PlayImpactSound() { PlaySound(ESound::RangerGrenadeExplosion); }

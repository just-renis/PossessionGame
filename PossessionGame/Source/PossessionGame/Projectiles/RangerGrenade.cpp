#include "RangerGrenade.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Enums/ESound.h"

ARangerGrenade::ARangerGrenade()
{
	Sounds.Add(ESound::RangerGrenadeExplosion, CreateDefaultSubobject<UAudioComponent>(TEXT("grenadeExplosionSound")));
	Sounds[ESound::RangerGrenadeExplosion]->SetAutoActivate(false);
	Sounds[ESound::RangerGrenadeExplosion]->SetupAttachment(Mesh);
}

void ARangerGrenade::PlayImpactSound() { PlaySound(ESound::RangerGrenadeExplosion); }

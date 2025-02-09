#include "EMPGrenade.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Enums/ESound.h"

AEMPGrenade::AEMPGrenade()
{
	Sounds.Add(ESound::TricksterEMPImpact, CreateDefaultSubobject<UAudioComponent>(TEXT("EMPImpactSound")));
	Sounds[ESound::TricksterEMPImpact]->SetAutoActivate(false);
	Sounds[ESound::TricksterEMPImpact]->SetupAttachment(Mesh);
}

void AEMPGrenade::PlayImpactSound()
{
	PlaySound(ESound::TricksterEMPImpact);
}

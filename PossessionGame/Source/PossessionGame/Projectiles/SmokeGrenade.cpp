#include "SmokeGrenade.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Enums/ESound.h"

ASmokeGrenade::ASmokeGrenade()
{
	Sounds.Add(ESound::TricksterSmokeImpact, CreateDefaultSubobject<UAudioComponent>(TEXT("smokeImpactSound")));
	Sounds[ESound::TricksterSmokeImpact]->SetAutoActivate(false);
	Sounds[ESound::TricksterSmokeImpact]->SetupAttachment(Mesh);
	Sounds.Add(ESound::TricksterSmokeRelease, CreateDefaultSubobject<UAudioComponent>(TEXT("smokeReleaseSound")));
	Sounds[ESound::TricksterSmokeRelease]->SetAutoActivate(false);
	Sounds[ESound::TricksterSmokeRelease]->SetupAttachment(Mesh);
}

void ASmokeGrenade::PlayImpactSound()
{
	PlaySound(ESound::TricksterSmokeImpact);
	PlaySound(ESound::TricksterSmokeRelease);
}

#include "SmokeGrenade.h"
#include "NavModifierComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Enums/ESound.h"

ASmokeGrenade::ASmokeGrenade()
{
	SmokeNavModifier = CreateDefaultSubobject<UNavModifierComponent>("navModifier");
	Sounds.Add(ESound::TricksterSmokeImpact, CreateDefaultSubobject<UAudioComponent>(TEXT("smokeImpactSound")));
	Sounds[ESound::TricksterSmokeImpact]->SetAutoActivate(false);
	Sounds[ESound::TricksterSmokeImpact]->SetupAttachment(Mesh);
	Sounds.Add(ESound::TricksterSmokeRelease, CreateDefaultSubobject<UAudioComponent>(TEXT("smokeReleaseSound")));
	Sounds[ESound::TricksterSmokeRelease]->SetAutoActivate(false);
	Sounds[ESound::TricksterSmokeRelease]->SetupAttachment(Mesh);
}

void ASmokeGrenade::PlayImpactSound()
{
	SmokeNavModifier->SetAreaClass(OriginalNavAreaClass);
	UKismetSystemLibrary::K2_SetTimer(this, "OnSmokeEnd", 4.f, false);
	PlaySound(ESound::TricksterSmokeImpact);
	PlaySound(ESound::TricksterSmokeRelease);
}

void ASmokeGrenade::Init(FProjectileData& InProjectileData, const FRotator& Rotation, const FVector& Scale)
{
	Super::Init(InProjectileData, Rotation, Scale);
	if (!OriginalNavAreaClass) OriginalNavAreaClass = SmokeNavModifier->AreaClass;
}

void ASmokeGrenade::OnSmokeEnd() const { SmokeNavModifier->SetAreaClass(nullptr); }

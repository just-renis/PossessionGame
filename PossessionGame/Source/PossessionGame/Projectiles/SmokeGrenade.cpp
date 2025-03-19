#include "SmokeGrenade.h"
#include "NavModifierComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Enums/ESound.h"

ASmokeGrenade::ASmokeGrenade()
{
	SmokeNavModifier = CreateDefaultSubobject<UNavModifierComponent>("navModifier");
	Sounds.Add(ESound::TricksterPoisonImpact, CreateDefaultSubobject<UAudioComponent>(TEXT("smokeImpactSound")));
	Sounds[ESound::TricksterPoisonImpact]->SetAutoActivate(false);
	Sounds[ESound::TricksterPoisonImpact]->SetupAttachment(Mesh);
	Sounds.Add(ESound::TricksterPoisonRelease, CreateDefaultSubobject<UAudioComponent>(TEXT("smokeReleaseSound")));
	Sounds[ESound::TricksterPoisonRelease]->SetAutoActivate(false);
	Sounds[ESound::TricksterPoisonRelease]->SetupAttachment(Mesh);
}

void ASmokeGrenade::PlayImpactSound()
{
	SmokeNavModifier->SetAreaClass(OriginalNavAreaClass);
	UKismetSystemLibrary::K2_SetTimer(this, "OnSmokeEnd", 4.f, false);
	PlaySound(ESound::TricksterPoisonImpact);
	PlaySound(ESound::TricksterPoisonRelease);
}

void ASmokeGrenade::Init(FProjectileData& InProjectileData, const FRotator& Rotation, const FVector& Scale, const int32 InWhich)
{
	Super::Init(InProjectileData, Rotation, Scale, InWhich);
	if (!OriginalNavAreaClass) OriginalNavAreaClass = SmokeNavModifier->AreaClass;
}

void ASmokeGrenade::OnSmokeEnd() const { SmokeNavModifier->SetAreaClass(nullptr); }

#include "Minion.h"
#include "Components/AudioComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"

AMinion::AMinion()
{
	Sounds.Add(ESound::MinionSpawn, CreateDefaultSubobject<UAudioComponent>(TEXT("spawnSound")));
	Sounds[ESound::MinionSpawn]->SetAutoActivate(false);
	Sounds.Add(ESound::MinionDeath, CreateDefaultSubobject<UAudioComponent>(TEXT("deathSound")));
	Sounds[ESound::MinionDeath]->SetAutoActivate(false);
}

void AMinion::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::MinionWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void AMinion::AI_PlayDeathSound() { PlaySound(ESound::MinionDeath); }

void AMinion::AI_PlaySpawnSound() { PlaySound(ESound::MinionSpawn); }

void AMinion::BeginPlay()
{
	CharacterName = ECharacterType::Minion;
	Super::BeginPlay();
	UKismetSystemLibrary::K2_SetTimer(this, "AI_PlaySpawnSound", 3.f + UKismetMathLibrary::RandomInteger(4), false);
}

void AMinion::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (bIsPossessed) UKismetSystemLibrary::K2_ClearTimer(this, "AI_PlaySpawnSound");
}

void AMinion::ClearTimers()
{
	Super::ClearTimers();
	UKismetSystemLibrary::K2_ClearTimer(this, "AI_PlaySpawnSound");
}

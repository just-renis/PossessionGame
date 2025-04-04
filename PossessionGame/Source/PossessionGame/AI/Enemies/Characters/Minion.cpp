#include "Minion.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"

AMinion::AMinion()
{
	Sounds.Add(ESound::MinionSpawn, CreateDefaultSubobject<UAudioComponent>(TEXT("spawnSound")));
	Sounds[ESound::MinionSpawn]->SetAutoActivate(false);
	Sounds[ESound::MinionSpawn]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::MinionDeath, CreateDefaultSubobject<UAudioComponent>(TEXT("deathSound")));
	Sounds[ESound::MinionDeath]->SetAutoActivate(false);
	Sounds[ESound::MinionDeath]->SetupAttachment(GetMesh());
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
}

void AMinion::DestroyAI()
{
	Super::DestroyAI();
	if (!Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot()->GetIsTutorialFinished())
	{
		(void)DelegateManager->StartTutorialDroneMoveToLocation.ExecuteIfBound(2);
		(void)DelegateManager->StopShowingHint.ExecuteIfBound();
	}
}

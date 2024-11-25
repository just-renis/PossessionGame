#include "WaveManager.h"
#include "DelegateManager.h"
#include "PrimaryDataAssetsManager.h"
#include "WeaponManager.h"
#include "WidgetManager.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/AssetManager.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_CharacterContainer.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_WeaponContainer.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/UI/WaveManagerHUD.h"
#include "PossessionGame/Utilities/Global Utilities/SpawnLocation.h"

UWaveManager::UWaveManager() { SetupSounds(); }

void UWaveManager::Init(TObjectPtr<UDelegateManager> InDelegateManager)
{
	DelegateManager = InDelegateManager;
	InitDelegates();
	SetupSpawnLocations();
	SetupWaveManagerUI();
	SetupTimers();
}

void UWaveManager::InitDelegates() { DelegateManager->OnEnemyDeath.AddUObject(this, &UWaveManager::UpdateEnemiesAmount); }

void UWaveManager::SetupWaveManagerSettings(TArray<ECharacterType>& EnemiesThatAreSpawnable, ECharacterType FirstEnemy, int32 EnemiesAmountOnFirstWave)
{
	FirstEnemyToSpawn = FirstEnemy;
	EnemiesPerWave = EnemiesAmountOnFirstWave;
	EnemiesLeftToKill = EnemiesPerWave;
	WaveManagerHUD->UpdateEnemiesAmount(EnemiesLeftToKill + AliveEnemiesAmount);
	for (int32 i = 0; i < EnemiesThatAreSpawnable.Num(); i++) EnemyTypes.Add(EnemiesThatAreSpawnable[i]);
	if (EnemiesThatAreSpawnable.Num() < 1) EnemyTypes.Add(ECharacterType::Spy);
}

bool UWaveManager::IsGameRunning() { return IsValid(this) && GetWorld() && !GetWorld()->HasAnyFlags(RF_BeginDestroyed | RF_FinishDestroyed); }

EWeapon UWaveManager::GetWeaponCode(ECharacterType EnemyType)
{
	switch (EnemyType)
	{
		case ECharacterType::Tank: return EWeapon::TankWeapon;
		case ECharacterType::Minion: return EWeapon::MinionWeapon;
		case ECharacterType::Ranger: return EWeapon::RangerWeapon;
		case ECharacterType::Spy: return EWeapon::SpyWeapon;
		case ECharacterType::Trickster: return EWeapon::TricksterWeapon;
		case ECharacterType::Samurai: return EWeapon::SamuraiWeapon;
		default: return EWeapon::None;
	}
}

EWeapon UWaveManager::GetSingularWeapon(EWeapon Weapon)
{
	switch (Weapon)
	{
		case EWeapon::MinionWeapon: return EWeapon::MinionSingularWeapon;
		case EWeapon::TricksterWeapon: return EWeapon::TricksterSingularWeapon;
		default: return EWeapon::None;
	}
}

void UWaveManager::SetupTimers()
{
	UpdateCountdown();
	UKismetSystemLibrary::K2_SetTimer(this, "UpdateCountdown", 1.f, true);
}

void UWaveManager::StartWave()
{
	if (WaveManagerHUD)
	{
		WaveManagerHUD->ToggleCountdownText();
		WaveManagerHUD->ToggleEnemiesLeftText();
		WaveManagerHUD->ToggleCurrentWaveText();
	}
	PlaySound(ESound::WaveStart);
}

void UWaveManager::InitiateNextWave()
{
	SetupNextWave();
	if (WaveManagerHUD)
	{
		SetupNextWaveUI();
		InitiateWave();
	}
}

void UWaveManager::SetupNextWave()
{
	CurrentWave++;
	EnemiesPerWave += FMath::RandRange(2, 5);
	EnemiesLeftToKill = EnemiesPerWave;
	CountdownSeconds = 3.f;
	UpdateCountdown();
	UKismetSystemLibrary::K2_ClearTimer(this, "HandleSpawning");
	UKismetSystemLibrary::K2_SetTimer(this, "UpdateCountdown", 1.f, true);
}

void UWaveManager::SetupNextWaveUI()
{
	WaveManagerHUD->ToggleCurrentWaveText();
	WaveManagerHUD->ToggleEnemiesLeftText();
	WaveManagerHUD->UpdateCurrentWaveText(CurrentWave);
	WaveManagerHUD->UpdateEnemiesAmount(EnemiesLeftToKill + AliveEnemiesAmount);
}

void UWaveManager::SetupWaveManagerUI()
{
	WaveManagerHUD = GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UWaveManagerHUD>(EWidget::WaveManagerHUD);
	WaveManagerHUD->UpdateCountdown(CountdownSeconds);
	WaveManagerHUD->UpdateEnemiesAmount(EnemiesLeftToKill + AliveEnemiesAmount);
	WaveManagerHUD->UpdateCurrentWaveText(CurrentWave);
}

void UWaveManager::SetupSpawnLocations()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnLocation::StaticClass(), Actors);
	for (TObjectPtr<AActor> Actor : Actors) SpawnLocations.Add(Cast<ASpawnLocation>(Actor));
}

void UWaveManager::SpawnEnemy(ECharacterType EnemyType)
{
	if (SpawnLocations[FMath::RandRange(0, SpawnLocations.Num() - 1)]->SpawnEnemy(Enemies[EnemyType]))
	{
		AliveEnemiesAmount++;
		EnemiesLeftToKill--;
		bIsSpawnInProcess = false;
	}
}

void UWaveManager::SpawnLogic()
{
	if (!bIsSpawnInProcess)
	{
		ECharacterType EnemyType;
		if (FIRSTPOSSESSABLE)
		{
			EnemyType = FirstEnemyToSpawn;
			FIRSTPOSSESSABLE = false;
		}
		else EnemyType = EnemyTypes[FMath::RandRange(0, EnemyTypes.Num() - 1)];
		!Enemies.Contains(EnemyType) ? AsyncLoadAndSpawnEnemy(EnemyType) : SpawnEnemy(EnemyType);
	}
}

void UWaveManager::AsyncLoadAndSpawnEnemy(ECharacterType EnemyType)
{
	bIsSpawnInProcess = true;
	TObjectPtr<UPrimaryDataAssetsManager> AssetsManager = GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>();
	if (AssetsManager->GetCanAsyncLoadHappen())
	{
		TArray<FSoftObjectPath> PathsToLoad;
		PathsToLoad.Add(AssetsManager->GetCharacterData(EnemyType)->CharacterClass.ToSoftObjectPath());
		PathsToLoad.Add(AssetsManager->GetCharacterData(EnemyType)->CharacterMesh.ToSoftObjectPath());
		EWeapon Weapon = GetWeaponCode(EnemyType);
		PathsToLoad.Add(AssetsManager->GetWeaponData(Weapon)->WeaponClass.ToSoftObjectPath());
		PathsToLoad.Add(AssetsManager->GetWeaponData(Weapon)->HandsMesh.ToSoftObjectPath());
		EWeapon SingularWeapon = GetSingularWeapon(Weapon);
		if (SingularWeapon != EWeapon::None)
		{
			PathsToLoad.Add(AssetsManager->GetWeaponData(SingularWeapon)->WeaponClass.ToSoftObjectPath());
			PathsToLoad.Add(AssetsManager->GetWeaponData(SingularWeapon)->WeaponMesh.ToSoftObjectPath());
		}
		else PathsToLoad.Add(AssetsManager->GetWeaponData(Weapon)->WeaponMesh.ToSoftObjectPath());
		TSharedPtr<FStreamableHandle> Handle = UAssetManager::GetStreamableManager().RequestAsyncLoad(PathsToLoad, [this, SingularWeapon, Weapon, EnemyType, AssetsManager]
		{
			if (IsGameRunning()) AsyncLoadCallback(SingularWeapon, Weapon, EnemyType, AssetsManager);
		});
		AssetsManager->AddAsyncHandle(Handle);
	}
}

void UWaveManager::PlaySound(ESound SoundName)
{
	Sounds[SoundName]->Sound ? UGameplayStatics::PlaySound2D(GetWorld(), Sounds[SoundName]->Sound) : GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->LoadAndSetSound(Sounds[SoundName], SoundName, true);
}

void UWaveManager::SetupSounds()
{
	Sounds.Add(ESound::WaveStart, CreateDefaultSubobject<UAudioComponent>(TEXT("waveStartSound")));
	Sounds[ESound::WaveStart]->SetAutoActivate(false);
	Sounds.Add(ESound::WaveCountdown, CreateDefaultSubobject<UAudioComponent>(TEXT("waveCountdownSound")));
	Sounds[ESound::WaveCountdown]->SetAutoActivate(false);
}

void UWaveManager::AsyncLoadCallback(EWeapon SingularWeapon, EWeapon Weapon, ECharacterType EnemyType, TObjectPtr<UPrimaryDataAssetsManager> AssetsManager)
{
	Enemies.Add(EnemyType, Cast<UClass>(AssetsManager->GetCharacterData(EnemyType)->CharacterClass.ToSoftObjectPath().TryLoad()));
	if (SingularWeapon != EWeapon::None)
	{
		GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->AddWeapon(SingularWeapon, Cast<UClass>(AssetsManager->GetWeaponData(SingularWeapon)->WeaponClass.ToSoftObjectPath().TryLoad()));
	}
	GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->AddWeapon(Weapon, Cast<UClass>(AssetsManager->GetWeaponData(Weapon)->WeaponClass.ToSoftObjectPath().TryLoad()));
	SpawnEnemy(EnemyType);
}

void UWaveManager::UpdateCountdown()
{
	CountdownSeconds--;
	if (WaveManagerHUD) WaveManagerHUD->UpdateCountdown(CountdownSeconds);
	PlaySound(ESound::WaveCountdown);
	if (CountdownSeconds <= 0)
	{
		StartWave();
		UKismetSystemLibrary::K2_ClearTimer(this, "UpdateCountdown");
		UKismetSystemLibrary::K2_SetTimer(this, "HandleSpawning", SpawnRate, true);
	}
}

void UWaveManager::HandleSpawning()
{
	if (EnemiesLeftToKill > 0 && AliveEnemiesAmount < EnemiesOnSceneLimit) SpawnLogic();
	else if (EnemiesLeftToKill <= 0 && AliveEnemiesAmount <= 0) InitiateNextWave();
}

void UWaveManager::InitiateWave() { WaveManagerHUD->ToggleCountdownText(); }

void UWaveManager::UpdateEnemiesAmount()
{
	AliveEnemiesAmount--;
	if (WaveManagerHUD) WaveManagerHUD->UpdateEnemiesAmount(EnemiesLeftToKill + AliveEnemiesAmount);
}
#include "WaveManager.h"
#include "DelegateManager.h"
#include "PrimaryDataAssetsManager.h"
#include "WeaponManager.h"
#include "WidgetManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/AssetManager.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_CharacterContainer.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_WeaponContainer.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/UI/MainWidgets/UpgradeSelectionUI.h"
#include "PossessionGame/UI/MainWidgets/WaveManagerHUD.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/SpawnLocation.h"

void UWaveManager::Init(const bool bIsTutorialFinished, const TObjectPtr<UDelegateManager>& InDelegateManager)
{
	DelegateManager = InDelegateManager;
	bTutorialIsFinished = bIsTutorialFinished;
	SetupSpawnLocations();
	DelegateManager->OnGameModeInitiator.AddUObject(this, &UWaveManager::SetAndStartGameMode);
	if (!bTutorialIsFinished)
	{
		CurrentGameMode = EGameMode::TutorialMode1;
		DelegateManager->SpawnTutorialEnemy.BindUObject(this, &UWaveManager::AsyncLoadAndSpawnEnemy);
		DelegateManager->OnTutorialEnd.AddUObject(this, &UWaveManager::FinishTutorial);
		return;
	}
	InitMode();
}

void UWaveManager::InitMode()
{
	if (CurrentGameMode == EGameMode::WaveMode)
	{
		CurrentWave = 1;
		DelegateManager->OnEnemyDeath.AddUObject(this, &UWaveManager::UpdateEnemiesAmount);
		DelegateManager->OnPlayerDeath.AddUObject(this, &UWaveManager::FadeOutWavePanel);
		DelegateManager->OnUpgradeSelectionFinished.BindUObject(this, &UWaveManager::InitiateNextWave);
		SetupWaveManagerUI();
		SetupTimers();
	}
	else if (CurrentGameMode == EGameMode::HackTerminalsMode)
	{
		if (WaveManagerHUD) WaveManagerHUD->ToggleTaskProgress(true, CurrentGameMode);
		UKismetSystemLibrary::K2_SetTimer(this, "HandleSpawning", SpawnRate, true);
		UKismetSystemLibrary::K2_SetTimer(this, "UpdateTaskProgress", 0.5f, true);
	}
}

void UWaveManager::FinalizeMode()
{
	if (CurrentGameMode == EGameMode::WaveMode)
	{
		DelegateManager->OnEnemyDeath.RemoveAll(this);
        DelegateManager->OnPlayerDeath.RemoveAll(this);
        DelegateManager->OnUpgradeSelectionFinished.Unbind();
		FadeOutWavePanel();
	}
	else if (CurrentGameMode == EGameMode::HackTerminalsMode)
	{
		if (WaveManagerHUD) WaveManagerHUD->ToggleTaskProgress(false, CurrentGameMode);
		UKismetSystemLibrary::K2_ClearTimer(this, "HandleSpawning");
	}
	DelegateManager->OnGameModeFinished.Broadcast(CurrentGameMode);
}

void UWaveManager::SetupWaveManagerSettings(const TArray<ECharacterType>& InEnemiesThatAreSpawnable, const int32 InEnemiesAmountOnFirstWave)
{
	EnemiesPerWave = InEnemiesAmountOnFirstWave;
	EnemiesLeftToKill = EnemiesPerWave;
	if (WaveManagerHUD) WaveManagerHUD->UpdateEnemiesAmount(EnemiesLeftToKill + AliveEnemiesAmount);
	for (int32 i = 0; i < InEnemiesThatAreSpawnable.Num(); i++) EnemyTypes.Add(InEnemiesThatAreSpawnable[i]);
	if (InEnemiesThatAreSpawnable.Num() < 1) EnemyTypes.Add(ECharacterType::Ranger);
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
		case ECharacterType::Guardian: return EWeapon::GuardianWeapon;
		default: return EWeapon::None;
	}
}

EWeapon UWaveManager::GetSingularWeapon(EWeapon Weapon)
{
	switch (Weapon)
	{
		case EWeapon::MinionWeapon: return EWeapon::MinionSingularWeapon;
		case EWeapon::TricksterWeapon: return EWeapon::TricksterSingularWeapon;
		case EWeapon::GuardianWeapon: return EWeapon::GuardianSingularWeapon;
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
	if (++CurrentWave < 2)
	{
		bWaveNotStarted = false;
		SetupNextWave();
		if (WaveManagerHUD)
		{
			SetupNextWaveUI();
			InitiateWave();
		}
	}
	else FinalizeMode();
}

void UWaveManager::SetupNextWave()
{
	//EnemiesPerWave += FMath::RandRange(2, 5);
	EnemiesLeftToKill = EnemiesPerWave;
	CountdownSeconds = 4.f;
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
	for (TObjectPtr<AActor> Actor : Actors)
	{
		TObjectPtr<ASpawnLocation> SpawnLocation = Cast<ASpawnLocation>(Actor);
		SpawnLocations.Add(SpawnLocation->GetGameMode());
		SpawnLocations[SpawnLocation->GetGameMode()].SpawnLocations.Add(SpawnLocation);
	}
}

void UWaveManager::SpawnEnemy(const ECharacterType InEnemyType)
{
	if (!SpawnLocations.Find(CurrentGameMode)) return;
	if (!bTutorialIsFinished) TutorialHandleSpawning(InEnemyType);
	else if (SpawnLocations[CurrentGameMode].SpawnLocations[FMath::RandRange(0, SpawnLocations[CurrentGameMode].SpawnLocations.Num() - 1)]->SpawnEnemy(Enemies[InEnemyType]))
	{
		AliveEnemiesAmount++;
		EnemiesLeftToKill--;
	}
	bIsSpawnInProcess = false;
}

void UWaveManager::SpawnLogic()
{
	if (!bIsSpawnInProcess)
	{
		const ECharacterType EnemyType = EnemyTypes[FMath::RandRange(0, EnemyTypes.Num() - 1)];
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

void UWaveManager::PlaySound(const ESound InSoundName) const { GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(InSoundName); }

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

void UWaveManager::FadeOutWavePanel() const { if (WaveManagerHUD) WaveManagerHUD->PlayWavePanelFadeOutAnim(); }

void UWaveManager::WaveCompleted()
{
	if (!WaveManagerHUD) return;
	bWaveNotStarted = true;
	WaveManagerHUD->PlayWaveCompletedAnim();
	UKismetSystemLibrary::K2_SetTimer(this, "ShowUpgradeSelection", WaveManagerHUD->GetWaveAnnouncementEndTime(), false);
}

void UWaveManager::TutorialHandleSpawning(const ECharacterType InEnemyType)
{
	for (int32 i = 0; i < SpawnLocations[CurrentGameMode].SpawnLocations.Num(); i++)
	{
		if (CurrentGameMode == EGameMode::TutorialMode2) SpawnLocations[CurrentGameMode].SpawnLocations[i]->SpawnEnemy(Enemies[InEnemyType])->AI_Disable();
		else
		{
			SpawnLocations[CurrentGameMode].SpawnLocations[i]->SpawnEnemy(Enemies[InEnemyType]);
			CurrentGameMode = EGameMode::TutorialMode2;
		}
	}
}

void UWaveManager::SetAndStartGameMode(const EGameMode InGameMode)
{
	CurrentGameMode = InGameMode;
	InitMode();
}

void UWaveManager::FinishTutorial()
{
	bTutorialIsFinished = true;
	DelegateManager->OnGameModeFinished.Broadcast(CurrentGameMode);
}

void UWaveManager::ShowUpgradeSelection() { GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UUpgradeSelectionUI>(EWidget::UpgradeSelectionUI)->OpenUI(); }

void UWaveManager::UpdateCountdown()
{
	if (WaveManagerHUD)
	{
		WaveManagerHUD->UpdateCountdown(--CountdownSeconds);
		WaveManagerHUD->PlayCountdownAnim();
	}
	if (CountdownSeconds >= 0) PlaySound(ESound::WaveCountdown);
	else
	{
		StartWave();
		UKismetSystemLibrary::K2_ClearTimer(this, "UpdateCountdown");
		UKismetSystemLibrary::K2_SetTimer(this, "HandleSpawning", SpawnRate, true);
	}
}

void UWaveManager::HandleSpawning()
{
	if (CurrentGameMode == EGameMode::HackTerminalsMode && bUnlimitedSpawning && AliveEnemiesAmount < EnemiesOnSceneLimit) SpawnLogic();
	else if (CurrentGameMode == EGameMode::WaveMode)
	{
		if (EnemiesLeftToKill > 0 && AliveEnemiesAmount < EnemiesOnSceneLimit) SpawnLogic();
		else if (!bWaveNotStarted && EnemiesLeftToKill <= 0 && AliveEnemiesAmount <= 0) WaveCompleted();
	}
}

void UWaveManager::InitiateWave() { WaveManagerHUD->ToggleCountdownText(); }

void UWaveManager::UpdateEnemiesAmount()
{
	AliveEnemiesAmount--;
	if (CurrentGameMode == EGameMode::HackTerminalsMode && AliveEnemiesAmount <= 0) FinalizeMode();
	if (WaveManagerHUD) WaveManagerHUD->UpdateEnemiesAmount(EnemiesLeftToKill + AliveEnemiesAmount);
}

void UWaveManager::UpdateTaskProgress()
{
	TerminalHackTimer += 0.5f;
	if (WaveManagerHUD) WaveManagerHUD->UpdateTaskProgress(TerminalHackTimer, TerminalHackRequired);
	if (TerminalHackTimer >= TerminalHackRequired)
	{
		bUnlimitedSpawning = false;
		UKismetSystemLibrary::K2_ClearTimer(this, "UpdateTaskProgress");
	}
}

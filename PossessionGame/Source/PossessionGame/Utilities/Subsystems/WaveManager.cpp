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
	AssetsManager = GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>();
	bTutorialIsFinished = bIsTutorialFinished;
	SetupSpawnLocations();
	DelegateManager->OnGameModeInitiator.AddUObject(this, &UWaveManager::SetAndStartGameMode);
	DelegateManager->OnTerminalInteraction.AddUObject(this, &UWaveManager::StartTerminalHack);
	DelegateManager->OnEnemyDeath.AddUObject(this, &UWaveManager::UpdateEnemiesAmount);
	DelegateManager->OnUsbInserted.BindUObject(this, &UWaveManager::UpdateTaskProgress);
	if (bTutorialIsFinished) return;
	CurrentGameMode = EGameMode::TutorialMode1;
	DelegateManager->SpawnTutorialEnemy.BindUObject(this, &UWaveManager::AsyncLoadAndSpawnEnemy);
	DelegateManager->OnTutorialEnd.AddUObject(this, &UWaveManager::FinishTutorial);
}

void UWaveManager::InitMode()
{
	if (CurrentGameMode == EGameMode::WaveMode)
	{
		CurrentWave = 1;
		DelegateManager->OnPlayerDeath.AddUObject(this, &UWaveManager::FadeOutWavePanel);
		DelegateManager->OnUpgradeSelectionFinished.BindUObject(this, &UWaveManager::InitiateNextWave);
		SetupWaveManagerUI();
		SetupTimers();
	}
	else if (CurrentGameMode == EGameMode::HackTerminalsMode)
	{
		AssetsManager->PlaySound2D(ESound::GeneratorDisableAnnouncement);
		DelegateManager->OnHackModeStart.Broadcast();
	}
	else if (CurrentGameMode == EGameMode::ChargeTerminalMode)
	{
		AssetsManager->PlaySound2D(ESound::TaskAnnouncement);
		bUnlimitedSpawning = true;
		if (WaveManagerHUD) WaveManagerHUD->ToggleTaskProgress(true, CurrentGameMode);
		UKismetSystemLibrary::K2_SetTimer(this, "HandleSpawning", SpawnRate * 3.f, true);
	}
}

void UWaveManager::FinalizeMode()
{
	if (CurrentGameMode == EGameMode::WaveMode)
	{
        DelegateManager->OnPlayerDeath.RemoveAll(this);
        DelegateManager->OnUpgradeSelectionFinished.Unbind();
		FadeOutWavePanel();
	}
	else if (CurrentGameMode == EGameMode::HackTerminalsMode)
	{
		ResetTaskProgress();
		UKismetSystemLibrary::K2_ClearTimer(this, "HandleSpawning");
	}
	else if (CurrentGameMode == EGameMode::ChargeTerminalMode)
	{
		ShowUpgradeSelection();
		(void)DelegateManager->OnChargeTerminalModeCompletion.ExecuteIfBound();
		ResetTaskProgress();
		UKismetSystemLibrary::K2_ClearTimer(this, "HandleSpawning");
	}
	DelegateManager->OnGameModeFinished.Broadcast(CurrentGameMode);
	(void)DelegateManager->StartTutorialDroneMoveToLocation.ExecuteIfBound(ConvertGameModeToDroneLocation(), true);
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

int32 UWaveManager::ConvertGameModeToDroneLocation() const
{
	switch (CurrentGameMode)
	{
		case EGameMode::WaveMode: return 3;
		case EGameMode::HackTerminalsMode: return 4;
		case EGameMode::ChargeTerminalMode: return 5;
		default: ;
	}
	return 0;
}

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
		case ECharacterType::MinionBoss: return EWeapon::MinionBossHandWeapon;
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
	AssetsManager->PlaySound2D(ESound::TaskAnnouncement);
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
		if (!SpawnLocations.Contains(SpawnLocation->GetGameMode())) SpawnLocations.Add(SpawnLocation->GetGameMode());
		SpawnLocations[SpawnLocation->GetGameMode()].SpawnLocations.Add(SpawnLocation);
	}
}

void UWaveManager::SpawnEnemy(const ECharacterType InEnemyType)
{
	if (InEnemyType == ECharacterType::MinionBoss) SpawnLocations[EGameMode::BossMode].SpawnLocations[0]->SpawnEnemy(Enemies[InEnemyType]);
	if (InEnemyType == ECharacterType::Guardian) SpawnLocations[EGameMode::None].SpawnLocations[0]->SpawnEnemy(Enemies[InEnemyType]);
	if (!SpawnLocations.Find(CurrentGameMode)) return;
	if (!bTutorialIsFinished) TutorialHandleSpawning(InEnemyType);
	else if (SpawnLocations[CurrentGameMode].SpawnLocations.Num() > 0)
	{
		const int32 LastIndex = SpawnLocations[CurrentGameMode].SpawnLocations.Num() - 1;
		for (int32 i = 0; i <= LastIndex; i++)
		{
			const int32 Index = FMath::RandRange(i, LastIndex);
			if (i != Index) SpawnLocations[CurrentGameMode].SpawnLocations.Swap(i, Index);
		}
		for (const TObjectPtr<ASpawnLocation>& Location : SpawnLocations[CurrentGameMode].SpawnLocations)
		{
			if (Location->SpawnEnemy(Enemies[InEnemyType]))
			{
				AliveEnemiesAmount++;
				EnemiesLeftToKill--;
				bIsSpawnInProcess = false;
				break;
			}
		}
	}
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
		TSharedPtr<FStreamableHandle> Handle = UAssetManager::GetStreamableManager().RequestAsyncLoad(PathsToLoad, [this, SingularWeapon, Weapon, EnemyType]
		{
			if (IsGameRunning()) AsyncLoadCallback(SingularWeapon, Weapon, EnemyType);
		});
		AssetsManager->AddAsyncHandle(Handle);
	}
}

void UWaveManager::PlaySound(const ESound InSoundName) const { GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(InSoundName); }

void UWaveManager::AsyncLoadCallback(EWeapon SingularWeapon, EWeapon Weapon, ECharacterType EnemyType)
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
	PlaySound(ESound::WaveCompleted);
	bWaveNotStarted = true;
	WaveManagerHUD->PlayAnnouncementAnim("Wave Completed");
	UKismetSystemLibrary::K2_SetTimer(this, "ShowUpgradeSelection", WaveManagerHUD->GetWaveAnnouncementEndTime(), false);
}

void UWaveManager::TutorialHandleSpawning(const ECharacterType InEnemyType)
{
	for (const TObjectPtr<ASpawnLocation> Location : SpawnLocations[CurrentGameMode].SpawnLocations)
	{
		if (const TObjectPtr<AEnemy> SpawnedEnemy = Location->SpawnEnemy(Enemies[InEnemyType]))
		{
			if (CurrentGameMode == EGameMode::TutorialMode2) SpawnedEnemy->AI_Disable();
			else CurrentGameMode = EGameMode::TutorialMode2;
			AliveEnemiesAmount++;
			bIsSpawnInProcess = false;
			break;
		}
	}
}

void UWaveManager::SetAndStartGameMode(const EGameMode InGameMode)
{
	CurrentGameMode = InGameMode;
	InitMode();
}

void UWaveManager::FinishTutorial() { bTutorialIsFinished = true; }

void UWaveManager::StartTerminalHack()
{
	AssetsManager->PlaySound2D(ESound::GeneratorInitiate);
	AssetsManager->PlaySound2D(ESound::TaskAnnouncement);
	bUnlimitedSpawning = true;
	if (WaveManagerHUD)
	{
		WaveManagerHUD->ToggleTaskProgress(true, CurrentGameMode);
		WaveManagerHUD->PlayAnnouncementAnim("Hack Initiated");
	}
	UKismetSystemLibrary::K2_SetTimer(this, "HandleSpawning", SpawnRate, true);
	UKismetSystemLibrary::K2_SetTimer(this, "UpdateTaskProgress", 0.5f, true);
}

void UWaveManager::FinishTerminalHack()
{
	if (WaveManagerHUD)
	{
		WaveManagerHUD->PlayAnnouncementAnim("Hack Completed");
		UKismetSystemLibrary::K2_SetTimer(this, "ShowUpgradeSelection", WaveManagerHUD->GetWaveAnnouncementEndTime(), false);
	}
	AssetsManager->PlaySound2D(ESound::GeneratorCompleted);
	TerminalHackProgress = 0.f;
	if (--TerminalsLeftToHack <= 0) FinalizeMode();
	else
	{
		UKismetSystemLibrary::K2_ClearTimer(this, "HandleSpawning");
		DelegateManager->OnTerminalHackFinish.Broadcast(CurrentGameMode);
	}
}

void UWaveManager::ResetTaskProgress() const
{
	if (!WaveManagerHUD) return;
	WaveManagerHUD->ToggleTaskProgress(false, CurrentGameMode);
	WaveManagerHUD->UpdateTaskProgress(0.f, 1.f);
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
	if (CurrentGameMode != EGameMode::WaveMode && bUnlimitedSpawning && AliveEnemiesAmount < EnemiesOnSceneLimit) SpawnLogic();
	else
	{
		if (EnemiesLeftToKill > 0 && AliveEnemiesAmount < EnemiesOnSceneLimit) SpawnLogic();
		else if (!bWaveNotStarted && EnemiesLeftToKill <= 0 && AliveEnemiesAmount <= 0) WaveCompleted();
	}
}

void UWaveManager::InitiateWave() { WaveManagerHUD->ToggleCountdownText(); }

void UWaveManager::UpdateEnemiesAmount(const int32 InXp)
{
	AliveEnemiesAmount--;
	if (CurrentGameMode == EGameMode::TutorialMode1 || CurrentGameMode == EGameMode::TutorialMode2) return;
	if (CurrentGameMode == EGameMode::WaveMode && WaveManagerHUD) WaveManagerHUD->UpdateEnemiesAmount(EnemiesLeftToKill + AliveEnemiesAmount);
	else if (!bUnlimitedSpawning && AliveEnemiesAmount <= 0)
	{
		if (CurrentGameMode == EGameMode::HackTerminalsMode) FinishTerminalHack();
		else FinalizeMode();
	}
}

void UWaveManager::UpdateTaskProgress()
{
	if (CurrentGameMode == EGameMode::HackTerminalsMode)
	{
		TerminalHackProgress += 0.5f;
		if (WaveManagerHUD) WaveManagerHUD->UpdateTaskProgress(TerminalHackProgress, TerminalHackRequired);
		if (TerminalHackProgress >= TerminalHackRequired)
		{
			bUnlimitedSpawning = false;
			if (!bUnlimitedSpawning && AliveEnemiesAmount <= 0) FinishTerminalHack();
			UKismetSystemLibrary::K2_ClearTimer(this, "UpdateTaskProgress");
		}
	}
	else
	{
		TerminalHackProgress += TerminalHackRequired / 4.f;
		if (WaveManagerHUD) WaveManagerHUD->UpdateTaskProgress(TerminalHackProgress, TerminalHackRequired);
		if (TerminalHackProgress >= TerminalHackRequired)
		{
			bUnlimitedSpawning = false;
			if (!bUnlimitedSpawning && AliveEnemiesAmount <= 0) FinalizeMode();
		}
	}
}

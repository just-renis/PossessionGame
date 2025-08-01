#include "MusicPlayer.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void AMusicPlayer::BeginPlay()
{
	Super::BeginPlay();
	AssetsManager = GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>();
	CurrentSoundtrack = Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot()->GetIsTutorialFinished() ? ESound::RestSoundtrack : ESound::TutorialIntroSoundtrack;
	const TObjectPtr<UDelegateManager> DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	DelegateManager->OnMusicSoundtrackChangeRequired.BindUObject(this, &AMusicPlayer::ChangeSoundtrack);
	DelegateManager->ChangeMusicPlayerState.BindUObject(this, &AMusicPlayer::ChangeState);
	PlayCurrentSoundtrack();
}

void AMusicPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(FadeInHandle);
	Super::EndPlay(EndPlayReason);
}

void AMusicPlayer::ChangeSoundtrack()
{
	if (CurrentSoundtrack == ESound::BattleModeSoundtrack) return;
	CurrentSoundtrack = ESound::BattleModeSoundtrack;
	GetAudioComponent()->FadeOut(0.5f, 0.f);
	GetWorldTimerManager().SetTimer(FadeInHandle, this, &AMusicPlayer::PlayCurrentSoundtrack, 0.5f, false);
}

void AMusicPlayer::PlayCurrentSoundtrack() const
{
	AssetsManager->ConfigureSound(GetAudioComponent(), nullptr, CurrentSoundtrack);
	GetAudioComponent()->FadeIn(0.5f, 1.f, 0.f);
}

void AMusicPlayer::ChangeState(const bool bState) const
{
	if (bState) GetAudioComponent()->Play();
	else GetAudioComponent()->Stop();
}

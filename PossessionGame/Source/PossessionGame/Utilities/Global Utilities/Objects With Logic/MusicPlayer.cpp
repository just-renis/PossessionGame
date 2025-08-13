#include "MusicPlayer.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

void AMusicPlayer::BeginPlay()
{
	Super::BeginPlay();
	if (bMusicPlayerPlaysDefaultSoundTrack) return;
	bIsTutorialFinished = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetIsTutorialFinished();
	AssetsManager = GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>();
	CurrentSoundtrack = bIsTutorialFinished ? ESound::RestSoundTrack : ESound::TutorialIntroSoundTrack;
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
	if (CurrentSoundtrack == ESound::BattleModeSoundTrack) return;
	if (CurrentSoundtrack == ESound::TutorialIntroSoundTrack) CurrentSoundtrack = ESound::TutorialChaosSoundTrack;
	else CurrentSoundtrack = ESound::BattleModeSoundTrack;
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

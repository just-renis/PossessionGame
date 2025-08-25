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
	const TObjectPtr<UDelegateManager> DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	DelegateManager->OnMusicSoundtrackChangeRequired.BindUObject(this, &AMusicPlayer::ChangeSoundtrack);
	DelegateManager->ChangeMusicPlayerState.BindUObject(this, &AMusicPlayer::ChangeState);
	CurrentSoundTrack = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetIsTutorialFinished() ? ESound::RestSoundTrack : ESound::TutorialIntroSoundTrack;
	SwitchSoundTrack();
}

void AMusicPlayer::ChangeSoundtrack(const ESound InSoundTrackName)
{
	if (!SoundTracksData.Contains(InSoundTrackName)) return;
	if (CurrentSoundTrack == InSoundTrackName) return;
	bNeedToChangeSoundTrack = true;
	CurrentSoundTrack = InSoundTrackName;
}

void AMusicPlayer::ChangeState(const bool bState) const
{
	if (bState) GetAudioComponent()->Play();
	else GetAudioComponent()->Stop();
}

void AMusicPlayer::BeatsPerMinuteProcess()
{
	if (++CurrentBeat >= MaxCurrentSoundTrackBeat) CurrentBeat = 0;
	if (!bNeedToChangeSoundTrack) return;
	if (CurrentBeat % 4 == 0) SwitchSoundTrack();
}

void AMusicPlayer::SwitchSoundTrack()
{
	if (const int32* BeatsPerMinute = SoundTracksData.Find(CurrentSoundTrack))
	{
		MaxCurrentSoundTrackBeat = *BeatsPerMinute;
		CurrentBeat = 0;
		bNeedToChangeSoundTrack = false;
		GetWorldTimerManager().SetTimer(BeatsProcessorHandle, this, &AMusicPlayer::BeatsPerMinuteProcess, 60.f / MaxCurrentSoundTrackBeat, true);
	}
	GetAudioComponent()->Stop();
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(GetAudioComponent(), nullptr, CurrentSoundTrack);
	GetAudioComponent()->Play();
}

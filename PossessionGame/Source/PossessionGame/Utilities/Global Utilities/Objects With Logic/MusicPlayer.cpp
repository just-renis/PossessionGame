#include "MusicPlayer.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void AMusicPlayer::BeginPlay()
{
	Super::BeginPlay();
	AssetsManager = GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>();
	const TObjectPtr<UDelegateManager> DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	DelegateManager->OnGameModeInitiator.AddUObject(this, &AMusicPlayer::ChangeSoundTrack);
	DelegateManager->ChangeMusicPlayerState.BindUObject(this, &AMusicPlayer::ChangeState);
}

void AMusicPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(FadeInHandle);
	Super::EndPlay(EndPlayReason);
}

void AMusicPlayer::ChangeSoundTrack(const EGameMode InGameMode)
{
	if (CurrentSoundtrack == ESound::BattleModeSoundtrack) return;
	CurrentSoundtrack = ESound::BattleModeSoundtrack;
	GetAudioComponent()->FadeOut(0.5f, 0.f);
	GetWorldTimerManager().SetTimer(FadeInHandle, this, &AMusicPlayer::OnFadeOutFinished, 0.5f, false);
}

void AMusicPlayer::OnFadeOutFinished() const
{
	AssetsManager->ConfigureSound(GetAudioComponent(), nullptr, CurrentSoundtrack);
	GetAudioComponent()->FadeIn(0.5f, 1.f, 0.f);
}

void AMusicPlayer::ChangeState(const bool bState) const
{
	if (bState) GetAudioComponent()->Play();
	else GetAudioComponent()->Stop();
}

#include "MusicPlayer.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void AMusicPlayer::BeginPlay()
{
	Super::BeginPlay();
	AssetsManager = GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGameModeInitiator.AddUObject(this, &AMusicPlayer::ChangeSoundTrack);
	if (!GetAudioComponent()) return;
	GetAudioComponent()->OnAudioFinished.AddDynamic(this, &AMusicPlayer::OnMusicFinished);
}

void AMusicPlayer::ChangeSoundTrack(const EGameMode InGameMode)
{
	if (CurrentSoundtrack == ESound::BattleModeSoundtrack) return;
	CurrentSoundtrack = ESound::BattleModeSoundtrack;
	AssetsManager->ConfigureSound(GetAudioComponent(), CurrentSoundtrack);
}

void AMusicPlayer::OnMusicFinished() { AssetsManager->ConfigureSound(GetAudioComponent(), ESound::RestSoundtrackLooping); }

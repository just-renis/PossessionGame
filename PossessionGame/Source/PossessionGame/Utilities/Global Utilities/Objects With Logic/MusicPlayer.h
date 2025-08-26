#pragma once

#include "CoreMinimal.h"
#include "Sound/AmbientSound.h"
#include "MusicPlayer.generated.h"

enum class ESound : uint16;

UCLASS()
class POSSESSIONGAME_API AMusicPlayer : public AAmbientSound
{
	GENERATED_BODY()

	bool bNeedToChangeSoundTrack = false;
	int32 CurrentBeat = 0;
	int32 MaxCurrentSoundTrackBeat = 0;
	ESound CurrentSoundTrack;
	UPROPERTY(EditAnywhere, Category = "Sound Tracks", meta = (AllowPrivateAccess = true))
	TMap<ESound, int32> SoundTracksData;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bMusicPlayerPlaysDefaultSoundTrack = false;
	UPROPERTY()
	FTimerHandle SwitchHandle;
	UPROPERTY()
	FTimerHandle BeatsProcessorHandle;

	virtual void BeginPlay() override;
	void ChangeSoundtrack(const ESound InSoundTrackName, const bool bInstantChange);
	void ChangeState(const bool bState) const;
	void BeatsPerMinuteProcess();
	void SwitchSoundTrack();
	void PlayOtherSoundTrack() const;
};

#pragma once

#include "CoreMinimal.h"
#include "Sound/AmbientSound.h"
#include "MusicPlayer.generated.h"

enum class ESound : uint16;
class UPrimaryDataAssetsManager;

UCLASS()
class POSSESSIONGAME_API AMusicPlayer : public AAmbientSound
{
	GENERATED_BODY()

	bool bIsTutorialFinished = false;
	ESound CurrentSoundtrack;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bMusicPlayerPlaysDefaultSoundTrack = false;
	UPROPERTY()
	TObjectPtr<UPrimaryDataAssetsManager> AssetsManager;
	UPROPERTY()
	FTimerHandle FadeInHandle;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void ChangeSoundtrack();
	void PlayCurrentSoundtrack() const;
	void ChangeState(const bool bState) const;
};

#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Enums/ESound.h"
#include "Sound/AmbientSound.h"
#include "MusicPlayer.generated.h"

class UPrimaryDataAssetsManager;
enum class EGameMode : uint8;

UCLASS()
class POSSESSIONGAME_API AMusicPlayer : public AAmbientSound
{
	GENERATED_BODY()

	ESound CurrentSoundtrack = ESound::RestSoundtrack;
	UPROPERTY()
	TObjectPtr<UPrimaryDataAssetsManager> AssetsManager;

	virtual void BeginPlay() override;
	void ChangeSoundTrack(const EGameMode InGameMode);
	UFUNCTION()
	void OnMusicFinished();
};

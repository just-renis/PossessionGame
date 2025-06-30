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
	UPROPERTY()
	FTimerHandle FadeInHandle;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void ChangeSoundTrack(const EGameMode InGameMode);
	void OnFadeOutFinished() const;
	void ChangeState(const bool bState) const;
};

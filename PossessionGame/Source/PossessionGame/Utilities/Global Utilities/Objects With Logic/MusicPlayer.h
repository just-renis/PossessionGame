#pragma once

#include "CoreMinimal.h"
#include "Sound/AmbientSound.h"
#include "MusicPlayer.generated.h"

enum class ESound : uint16;
class UPrimaryDataAssetsManager;
enum class EGameMode : uint8;

UCLASS()
class POSSESSIONGAME_API AMusicPlayer : public AAmbientSound
{
	GENERATED_BODY()

	ESound CurrentSoundtrack;
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

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialBot.generated.h"

class ATutorialDialogue;
enum class ESound : uint16;

UENUM()
enum class ETutorialBotAnimState
{
	Idle,
	ThumbsUp,
	Wave,
	Tutorial1,
	Tutorial2,
	Tutorial3,
	Tutorial4,
	Tutorial5,
	Tutorial6,
	Tutorial7,
	Tutorial8,
	Tutorial9,
	Tutorial10,
	Tutorial11,
	Tutorial12,
};

UCLASS()
class POSSESSIONGAME_API ATutorialBot : public AActor
{
	GENERATED_BODY()

	ATutorialBot();

	int32 TutorialDialogueIndex = 0;
	UPROPERTY(EditDefaultsOnly, Category = "Dialogue", meta = (AllowPrivateAccess = true))
	TSubclassOf<ATutorialDialogue> DialogueClass;
	UPROPERTY(EditDefaultsOnly, Category = "Skeleton", meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, Category = "Animations", meta = (AllowPrivateAccess = true))
	TMap<ETutorialBotAnimState, TObjectPtr<UAnimationAsset>> Animations;
	UPROPERTY()
	FTimerHandle HitVent1Handle;
	UPROPERTY()
	FTimerHandle HitVent2Handle;
	UPROPERTY()
	FTimerHandle HitVent3Handle;
	UPROPERTY()
	FTimerHandle BreakVentHandle;
	UPROPERTY()
	FTimerHandle LandOnHeadHandle;
	UPROPERTY()
	FTimerHandle AnimFinishedHandle;
	UPROPERTY()
	TObjectPtr<ATutorialDialogue> Dialogue;
	UPROPERTY()
	TMap<ESound, TObjectPtr<UAudioComponent>> Sounds;

	TObjectPtr<UAnimationAsset> GetAnimation(const ETutorialBotAnimState InAnimState);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void PlaySound(const ESound InSoundName);
	void PlayFirstAnim();
	void ClearTimers();
	void SetupDialogue();
	void StartNextDialogue();
	void SetupAndPlayNextAnimation(const ETutorialBotAnimState InAnimState, void (ATutorialBot::*InCallback)());
	void FirstAnimationFinished();
	void SecondAnimationFinished();
	void ThirdAnimationFinished();
	void FourthAnimationFinished();
	void FifthAnimationFinished();
	void SixthAnimationFinished();
};

using FAnimCallback = void (ATutorialBot::*)();
#pragma once

#include "CoreMinimal.h"
#include "TutorialBot.generated.h"

class ADroneWaypoint;
class UTutorialBotAnimationInstance;
class ATutorialDialogue;
enum class ESound : uint16;

UENUM()
enum class ETutorialBotAnimState
{
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
	Idle,
	ThumbsUp,
	Wave
};

UCLASS()
class POSSESSIONGAME_API ATutorialBot : public AActor
{
	GENERATED_BODY()

	ATutorialBot();

	bool bIsMoving = false;
	int32 TutorialDialogueIndex = 0;
	int32 CurrentTutorialAnimation = 0;
	int32 CurrentWaypointIndex = 0;
	FVector WaypointLocation = FVector::ZeroVector;
	UPROPERTY(EditDefaultsOnly, Category = "Movement", meta = (AllowPrivateAccess = true))
	float MovementSpeed = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "Dialogue", meta = (AllowPrivateAccess = true))
	TSubclassOf<ATutorialDialogue> DialogueClass;
	UPROPERTY(EditDefaultsOnly, Category = "Skeleton", meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, Category = "Animations", meta = (AllowPrivateAccess = true))
	TMap<ETutorialBotAnimState, TObjectPtr<UAnimMontage>> Animations;
	UPROPERTY()
	TObjectPtr<UTutorialBotAnimationInstance> AnimInstance;
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
	FTimerHandle TargetLocationUpdateHandle;
	UPROPERTY()
	FTimerHandle MoveToLocationHandle;
	UPROPERTY()
	TObjectPtr<ATutorialDialogue> Dialogue;
	UPROPERTY()
	TArray<TObjectPtr<ADroneWaypoint>> Waypoints;
	UPROPERTY()
	TMap<ESound, TObjectPtr<UAudioComponent>> Sounds;

	TObjectPtr<UAnimMontage> GetAnimation(const ETutorialBotAnimState InAnimState);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void PlaySound(const ESound InSoundName);
	void PlayFirstAnim();
	void ClearTimers();
	void SetupDialogue();
	void StartNextDialogue();
	void SetupAndPlayNextAnimation(const ETutorialBotAnimState InAnimState, void (ATutorialBot::*InCallback)(), const float InTimer = 0.f);
	void FirstAnimationFinished();
	void SecondAnimationFinished();
	void ThirdAnimationFinished();
	void FourthAnimationFinished();
	void FifthAnimationFinished();
	void SixthAnimationFinished();
	void SeventhAnimationFinished();
	void EighthAnimationFinished();
	void PlayNextAnimation();
	void NinthAnimationFinished();
	void TenthAnimationFinished();
	void EleventhAnimationFinished();
	void UpdateTargetLocation() const;
	void SetControlRigState(const bool bState);
	void RetrieveWaypoints();
	void SetupAndMoveToNextLocation(const bool bInstantMove = false);
	void MoveToLocation();
	void TwelfthAnimationFinished();
};

using FAnimCallback = void (ATutorialBot::*)();
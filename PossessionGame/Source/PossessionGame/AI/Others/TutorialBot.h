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
	Tutorial13,
	Tutorial14,
	Idle,
	ThumbsUp,
	Wave
};

UENUM()
enum class ETutorialBotExpression
{
	HappyWink,
	NeutralWink,
	Blink,
	Thinking,
	Sad,
	Happy,
	Grumpy,
	NeutralFace
};

UCLASS()
class POSSESSIONGAME_API ATutorialBot : public AActor
{
	GENERATED_BODY()

	ATutorialBot();

	bool bIsTutorialFinished = false;
	bool bIsTutorialPressKeyToContinueNeeded = false;
	int32 TutorialDialogueIndex = 0;
	int32 CurrentTutorialAnimation = 0;
	int32 CurrentWaypointIndex = 0;
	int32 PressKeyIndex = 0;
	FVector WaypointLocation = FVector::ZeroVector;
	ETutorialBotExpression CurrentExpression;
	UPROPERTY(EditDefaultsOnly, Category = "Expression | Random Blink", meta = (AllowPrivateAccess = true))
	float MinRange = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Expression | Random Blink", meta = (AllowPrivateAccess = true))
	float MaxRange = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "Vfx", meta = (AllowPrivateAccess = true))
	FRotator HitVentRotation;
	UPROPERTY(EditDefaultsOnly, Category = "Dialogue", meta = (AllowPrivateAccess = true))
	TSubclassOf<ATutorialDialogue> DialogueClass;
	UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> CrashVentCameraShake;
	UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> AlertCameraShake;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes", meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> TabletMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Animations", meta = (AllowPrivateAccess = true))
	TMap<ETutorialBotAnimState, TObjectPtr<UAnimMontage>> Animations;
	UPROPERTY()
	TObjectPtr<UTutorialBotAnimationInstance> AnimInstance;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_FaceExpression;
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
	FTimerHandle ResetControlRigStateHandle;
	UPROPERTY()
	FTimerHandle TabletVisibilityHandle;
	UPROPERTY()
	FTimerHandle RandomBlinkHandle;
	UPROPERTY()
	FTimerHandle EndBlinkHandle;
	UPROPERTY()
	FTimerHandle CheckDistanceToTargetHandle;
	UPROPERTY()
	FTimerHandle PressTabletHandle;
	UPROPERTY()
	TObjectPtr<ATutorialDialogue> Dialogue;
	UPROPERTY()
	TObjectPtr<AActor> Player;
	UPROPERTY()
	TArray<TObjectPtr<ADroneWaypoint>> Waypoints;

	FVector GetSocketLocation(const FName& InName) const;
	TObjectPtr<UAnimMontage> GetAnimation(const ETutorialBotAnimState InAnimState);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void PlaySound(const ESound InSoundName) const;
	void PlayFirstAnim();
	void ClearTimers();
	void SetupDialogue();
	void StartNextDialogue(const bool bShowPressAnyKeyToContinue = false);
	void SetupAndPlayNextAnimation(const ETutorialBotAnimState InAnimState, void (ATutorialBot::*InCallback)() = nullptr, const float InTimer = 0.f);
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
	void TutorialMoveToNextLocation();
	void MoveToNextLocation();
	void TwelfthAnimationFinished();
	void PlayCameraShake(const TSubclassOf<UCameraShakeBase>& InCameraShake) const;
	void AlertTriggered() const;
	void ThirteenthAnimationFinished();
	void FourteenthAnimationFinished();
	void PressAnyKeyPressed(const bool bState);
	void SetTabletVisibility(const bool bState) const;
	void SetExpression(const ETutorialBotExpression InExpressionName);
	void SetupNextRandomBlink();
	void TriggerBlink();
	void SetupExpression();
	void EnableCheckDistanceToTarget();
	void CheckDistanceToTarget();
	void StartTipDialogue() const;
	void PlayNextToGateAnimation();
	void WaveAnimationFinished();
};

using FAnimCallback = void (ATutorialBot::*)();
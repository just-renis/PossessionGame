#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/Interactable Objects/InteractableObject.h"
#include "TutorialBot.generated.h"

enum class ETutorialBotAnimState : uint8;
enum class ETutorialBotExpression : uint8;
class ADroneWaypoint;
class UTutorialBotAnimationInstance;
class ATutorialDialogue;
enum class ESound : uint16;
enum class EGameState : uint8;

UCLASS()
class POSSESSIONGAME_API ATutorialBot : public AInteractableObject
{
	GENERATED_BODY()

	ATutorialBot();

	bool bArtifactCollected = false;
	bool bIsTutorialFinished = false;
	bool bIsTutorialPressKeyToContinueNeeded = false;
	bool bCurrentWaypointAssociatedWithSecretRoom = false;
	int32 TutorialDialogueIndex = 0;
	int32 CurrentTutorialAnimation = 0;
	int32 CurrentWaypointIndex = 0;
	int32 PressKeyIndex = 0;
	FVector WaypointLocation = FVector::ZeroVector;
	ETutorialBotExpression CurrentExpression;
	EGameState CurrentGameState;
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
	void SetupDialogue(const FName& InSocketName);
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
	void PlayIdleAnimation();
	void SpawnMarker() const;
	virtual void Interact() override;
	void SetPlayerRef(const TObjectPtr<AActor>& InPlayerRef) { Player = InPlayerRef; }
	void SetupMeshOptimization() const;
	void SetCurrentGameState(const EGameState InGameState) { CurrentGameState = InGameState; }
	void SetArtifactCollected();
	void MoveToSecretRoomWaypoint();
	void SetExpressionAndPlayAnimation(const ETutorialBotAnimState InAnimState, const ETutorialBotExpression InExpression);
};

using FAnimCallback = void (ATutorialBot::*)();
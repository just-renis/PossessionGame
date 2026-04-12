#include "TutorialBot.h"
#include "TutorialBotAnimationInstance.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Navigation/DroneWaypoint.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/Enums/EGameState.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/ETutorialBotAnimState.h"
#include "PossessionGame/Enums/ETutorialBotExpression.h"
#include "PossessionGame/UI/MainWidgets/AbilitiesMenuUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/UI Actors/TutorialDialogue.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"
#include "PossessionGame/Utilities/Subsystems/MarkerManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Utilities/Subsystems/TutorialManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

ATutorialBot::ATutorialBot()
{
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	TabletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TabletMesh"));
	TabletMesh->SetVisibility(false);
}

FVector ATutorialBot::GetSocketLocation(const FName& InName) const
{
	if (!Mesh) return FVector::ZeroVector;
	return Mesh->GetSocketLocation(InName);
}

TObjectPtr<UAnimMontage> ATutorialBot::GetAnimation(const ETutorialBotAnimState InAnimState)
{
	if (const TObjectPtr<UAnimMontage> Animation = *Animations.Find(InAnimState)) return Animation;
	return nullptr;
}

void ATutorialBot::BeginPlay()
{
	Super::BeginPlay();
	SetupMeshOptimization();
	SetupExpression();
	RetrieveWaypoints();
	AnimInstance = Cast<UTutorialBotAnimationInstance>(Mesh->GetAnimInstance());
	bIsTutorialFinished = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetIsTutorialFinished();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnHijack.AddUObject(this, &ATutorialBot::SetPlayerRef);
	const int32 CurrentChapter = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetCurrentChapter();
	CurrentGameState = bIsArenaMode ? EGameState::ArenaMode : (CurrentChapter > 1 ? EGameState::DuringGame : EGameState::PreGame);
	if (!bIsTutorialFinished)
	{
		TabletMesh->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "TutorialTabletSocket");
		if (Collision) Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetWorld()->GetSubsystem<UTutorialManager>()->StartTutorialBotAnimation.BindUObject(this, &ATutorialBot::PlayNextAnimation);
		GetWorld()->GetSubsystem<UTutorialManager>()->OnShowPressKeyToContinue.AddUObject(this, &ATutorialBot::PressAnyKeyPressed);
		GetWorld()->GetSubsystem<UTutorialManager>()->OnFpsExplanationTriggered.BindUObject(this, &ATutorialBot::TutorialMoveToNextLocation);
		GetWorld()->GetSubsystem<UTutorialManager>()->OnMinionDeathCompleted.AddUObject(this, &ATutorialBot::PlayNextToGateAnimation);
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGateClosed.BindUObject(this, &ATutorialBot::TutorialMoveToNextLocation);
		SetupDialogue("BehindHeadSocket");
		PlayFirstAnim();
	}
	else
	{
		Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
		SetupDialogue("BodySocket");
		SetControlRigState(true);
		if (!bIsArenaMode && CurrentChapter > 1)
		{
			if (Collision) Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			SetupAndPlayNextAnimation(ETutorialBotAnimState::Idle);
		}
		else
		{
			TabletMesh->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "AbilitiesTabletSocket");
			SetTabletVisibility(true);
			SetupAndPlayNextAnimation(ETutorialBotAnimState::TabletHolding);
			GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGameModeInitialized.AddUObject(this, &ATutorialBot::SpawnMarker);
		}
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGateClosed.BindUObject(this, &ATutorialBot::MoveToNextLocation);
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnArtifactCollected.AddUObject(this, &ATutorialBot::SetArtifactCollected);
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTutorialBotTipDialogue.BindUObject(this, &ATutorialBot::SetExpressionAndPlayAnimation);
		EnableCheckDistanceToTarget();
	}
}

void ATutorialBot::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DelegateManager->OnHijack.RemoveAll(this);
	ClearTimers();
	Super::EndPlay(EndPlayReason);
}

void ATutorialBot::PlaySound(const ESound InSoundName) const
{
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySoundAtLocation(InSoundName, GetActorLocation());
}

void ATutorialBot::PlayFirstAnim()
{
	SetupAndPlayNextAnimation(ETutorialBotAnimState::Tutorial1, &ATutorialBot::FirstAnimationFinished);
	GetWorld()->GetTimerManager().SetTimer(HitVent1Handle, [this]
	{
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->SpawnVfx(EVfx::HitVent, nullptr, GetSocketLocation("HeadSocket"), HitVentRotation);
		PlaySound(ESound::BotHitVent);
	}, 8.62f, false);
	GetWorld()->GetTimerManager().SetTimer(HitVent2Handle, [this]
	{
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->SpawnVfx(EVfx::HitVent, nullptr, GetSocketLocation("HeadSocket"), HitVentRotation);
		PlaySound(ESound::BotHitVent);
	}, 9.59f, false);
	GetWorld()->GetTimerManager().SetTimer(HitVent3Handle, [this]
	{
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->SpawnVfx(EVfx::HitVent, nullptr, GetSocketLocation("HeadSocket"), HitVentRotation);
		PlaySound(ESound::BotHitVent);
	}, 10.42f, false);
	GetWorld()->GetTimerManager().SetTimer(BreakVentHandle, [this]
	{
		PlayCameraShake(CrashVentCameraShake);
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->SpawnVfx(EVfx::CrashVent, nullptr, GetSocketLocation("HeadSocket"));
		PlaySound(ESound::BotBreakVent);
	}, 11.69f, false);
	GetWorld()->GetTimerManager().SetTimer(LandOnHeadHandle, [this]
	{
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->SpawnVfx(EVfx::TutorialBotLandOnHead, nullptr, GetSocketLocation("HeadSocket"));
		PlaySound(ESound::BotLandOnHead);
	}, 11.92f, false);
}

void ATutorialBot::ClearTimers()
{
	GetWorld()->GetTimerManager().ClearTimer(HitVent1Handle);
	GetWorld()->GetTimerManager().ClearTimer(HitVent2Handle);
	GetWorld()->GetTimerManager().ClearTimer(HitVent3Handle);
	GetWorld()->GetTimerManager().ClearTimer(BreakVentHandle);
	GetWorld()->GetTimerManager().ClearTimer(LandOnHeadHandle);
	GetWorld()->GetTimerManager().ClearTimer(AnimFinishedHandle);
	GetWorld()->GetTimerManager().ClearTimer(TargetLocationUpdateHandle);
	GetWorld()->GetTimerManager().ClearTimer(ResetControlRigStateHandle);
	GetWorld()->GetTimerManager().ClearTimer(TabletVisibilityHandle);
	GetWorld()->GetTimerManager().ClearTimer(RandomBlinkHandle);
	GetWorld()->GetTimerManager().ClearTimer(EndBlinkHandle);
	GetWorld()->GetTimerManager().ClearTimer(CheckDistanceToTargetHandle);
	GetWorld()->GetTimerManager().ClearTimer(PressTabletHandle);
}

void ATutorialBot::SetupDialogue(const FName& InSocketName)
{
	Dialogue = GetWorld()->SpawnActor<ATutorialDialogue>(DialogueClass);
	Dialogue->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, InSocketName);
	Dialogue->SetVisibilityState(false);
}

void ATutorialBot::StartNextDialogue(const bool bShowPressAnyKeyToContinue)
{
	Dialogue->SetVisibilityState(true);
	Dialogue->StartDialogue(TutorialDialogueIndex++, bShowPressAnyKeyToContinue);
}

void ATutorialBot::SetupAndPlayNextAnimation(const ETutorialBotAnimState InAnimState, const FAnimCallback InCallback, const float InTimer)
{
	CurrentTutorialAnimation++;
	const TObjectPtr<UAnimMontage> Animation = GetAnimation(InAnimState);
	if (InCallback) GetWorld()->GetTimerManager().SetTimer(AnimFinishedHandle, this, InCallback, InTimer > 0.f ? InTimer : Animation->GetPlayLength());
	AnimInstance->Montage_Play(Animation);
}

void ATutorialBot::FirstAnimationFinished()
{
	StartNextDialogue();
	SetupAndPlayNextAnimation(ETutorialBotAnimState::Tutorial2, &ATutorialBot::SecondAnimationFinished);
}

void ATutorialBot::SecondAnimationFinished()
{
	Dialogue->SetVisibilityState(false);
	Dialogue->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "BodySocket");
	SetupAndPlayNextAnimation(ETutorialBotAnimState::Tutorial3, &ATutorialBot::ThirdAnimationFinished);
}

void ATutorialBot::ThirdAnimationFinished()
{
	StartNextDialogue();
	SetExpression(ETutorialBotExpression::Happy);
	SetupAndPlayNextAnimation(ETutorialBotAnimState::Tutorial4, &ATutorialBot::FourthAnimationFinished);
}

void ATutorialBot::FourthAnimationFinished()
{
	Dialogue->SetVisibilityState(false);
	SetExpression(ETutorialBotExpression::NeutralFace);
	SetupAndPlayNextAnimation(ETutorialBotAnimState::Tutorial5, &ATutorialBot::FifthAnimationFinished);
}

void ATutorialBot::FifthAnimationFinished()
{
	StartNextDialogue();
	SetupAndPlayNextAnimation(ETutorialBotAnimState::Tutorial6, &ATutorialBot::SixthAnimationFinished);
}

void ATutorialBot::SixthAnimationFinished()
{
	Dialogue->SetVisibilityState(false);
	SetupAndPlayNextAnimation(ETutorialBotAnimState::Tutorial7, &ATutorialBot::SeventhAnimationFinished);
	GetWorld()->GetTimerManager().SetTimer(PressTabletHandle, [this] { PlaySound(ESound::BotPressTablet); }, 1.4f, false);
	GetWorld()->GetTimerManager().SetTimer(TabletVisibilityHandle, [this] { SetTabletVisibility(true); }, 0.5f, false);
}

void ATutorialBot::SeventhAnimationFinished()
{
	StartNextDialogue();
	SetupAndPlayNextAnimation(ETutorialBotAnimState::Tutorial8, &ATutorialBot::EighthAnimationFinished);
	GetWorld()->GetTimerManager().SetTimer(TabletVisibilityHandle, [this] { SetTabletVisibility(false); }, 0.5f, false);
}

void ATutorialBot::EighthAnimationFinished()
{
	GetWorld()->GetSubsystem<UTutorialManager>()->OnShowHint.Broadcast();
	(void)GetWorld()->GetSubsystem<UTutorialManager>()->SpawnTutorialEnemy.ExecuteIfBound(ECharacterType::Guardian, EGameMode::TutorialMode1);
}

void ATutorialBot::PlayNextAnimation()
{
	switch (CurrentTutorialAnimation)
	{
		case 8:
			SetControlRigState(true);
			StartNextDialogue();
			SetExpression(ETutorialBotExpression::Happy);
			SetupAndPlayNextAnimation(ETutorialBotAnimState::Tutorial9, &ATutorialBot::NinthAnimationFinished, 2.5f);
		default: ;
	}
}

void ATutorialBot::NinthAnimationFinished()
{
	AlertTriggered();
	Dialogue->SetVisibilityState(false);
	SetControlRigState(false);
	SetExpression(ETutorialBotExpression::Sad);
	SetupAndPlayNextAnimation(ETutorialBotAnimState::Tutorial10, &ATutorialBot::TenthAnimationFinished);
}

void ATutorialBot::TenthAnimationFinished()
{
	SetControlRigState(true);
	StartNextDialogue();
	SetupAndPlayNextAnimation(ETutorialBotAnimState::Tutorial11, &ATutorialBot::EleventhAnimationFinished);
	GetWorld()->GetTimerManager().SetTimer(ResetControlRigStateHandle, [this] { SetControlRigState(false); }, 1.66f, false);
}

void ATutorialBot::EleventhAnimationFinished()
{
	Dialogue->SetVisibilityState(false);
	SetExpression(ETutorialBotExpression::NeutralFace);
	SetupAndPlayNextAnimation(ETutorialBotAnimState::Tutorial12, &ATutorialBot::TwelfthAnimationFinished);
}

void ATutorialBot::UpdateTargetLocation() const
{
	if (AnimInstance) AnimInstance->SetLookAtLocation(GetWorld()->GetSubsystem<UEnemyCommunicator>()->GetTargetLocation());
}

void ATutorialBot::SetControlRigState(const bool bState)
{
	if (!AnimInstance) return;
	AnimInstance->SetIsControlRigEnabled(bState);
	if (bState) GetWorld()->GetTimerManager().SetTimer(TargetLocationUpdateHandle, this, &ATutorialBot::UpdateTargetLocation, 0.02f, true);
	else GetWorld()->GetTimerManager().ClearTimer(TargetLocationUpdateHandle);
}

void ATutorialBot::RetrieveWaypoints()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADroneWaypoint::StaticClass(), Actors);
	for (TObjectPtr<AActor> Actor : Actors) Waypoints.Add(Cast<ADroneWaypoint>(Actor));
}

void ATutorialBot::TutorialMoveToNextLocation()
{
	if (CurrentWaypointIndex != 0) bIsTutorialPressKeyToContinueNeeded = true;
	MoveToNextLocation();
}

void ATutorialBot::MoveToNextLocation()
{
	if (Collision && Collision->GetCollisionEnabled() == ECollisionEnabled::QueryOnly) Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EnableCheckDistanceToTarget();
	SetExpression(ETutorialBotExpression::NeutralFace);
	Dialogue->SetVisibilityState(false);
	for (int32 i = 0; i < Waypoints.Num(); i++)
	{
		if (Waypoints[i]->GetOrder() != CurrentWaypointIndex) continue;
		bCurrentWaypointAssociatedWithSecretRoom = Waypoints[i]->GetIsAssociatedWithSecretRoom();
		if (bArtifactCollected && bCurrentWaypointAssociatedWithSecretRoom) MoveToSecretRoomWaypoint();
		else
		{
			SetActorLocationAndRotation(Waypoints[i]->GetActorLocation(), Waypoints[i]->GetActorRotation());
			CurrentGameState = Waypoints[i]->GetGameState();
		}
		break;
	}
	if (bIsTutorialFinished && CurrentWaypointIndex == 0)
	{
		SetTabletVisibility(false);
		PlayIdleAnimation();
	}
	if (bIsTutorialFinished) Dialogue->SetNextDialogueText(CurrentGameState);
	CurrentWaypointIndex++;
}

void ATutorialBot::TwelfthAnimationFinished()
{
	SetupAndPlayNextAnimation(ETutorialBotAnimState::Tutorial13, &ATutorialBot::ThirteenthAnimationFinished);
}

void ATutorialBot::PlayCameraShake(const TSubclassOf<UCameraShakeBase>& InCameraShake) const
{
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(InCameraShake);
}

void ATutorialBot::AlertTriggered() const
{
	PlayCameraShake(AlertCameraShake);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ErrorChaos);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::AlertSignalization);
	GetWorld()->GetSubsystem<UTutorialManager>()->OnAlertStart.Broadcast();
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnMusicSoundtrackChangeRequired.ExecuteIfBound(ESound::TutorialChaosSoundTrack, true);
}

void ATutorialBot::ThirteenthAnimationFinished()
{
	StartNextDialogue();
	SetupAndPlayNextAnimation(ETutorialBotAnimState::Tutorial14, &ATutorialBot::FourteenthAnimationFinished);
}

void ATutorialBot::FourteenthAnimationFinished()
{
	SetControlRigState(true);
	GetWorld()->GetSubsystem<UTutorialManager>()->OnShowPressKeyToContinue.Broadcast(true);
}

void ATutorialBot::PressAnyKeyPressed(const bool bState)
{
	if (bState) return;
	switch (PressKeyIndex)
	{
		case 0: case 1: case 3: StartNextDialogue(true); break;
		case 2:
			StartNextDialogue();
			GetWorld()->GetSubsystem<UTutorialManager>()->OnShowHint.Broadcast();
			(void)GetWorld()->GetSubsystem<UTutorialManager>()->EnableMeleeChecking.ExecuteIfBound();
			break;
		case 4: (void)GetWorld()->GetSubsystem<UTutorialManager>()->TriggerTutorialEnd.ExecuteIfBound();
		default: ;
	}
	PressKeyIndex++;
}

void ATutorialBot::SetTabletVisibility(const bool bState) const { if (TabletMesh) TabletMesh->SetVisibility(bState); }

void ATutorialBot::SetExpression(const ETutorialBotExpression InExpressionName)
{
	if (!MID_FaceExpression) return;
	if (InExpressionName != ETutorialBotExpression::Blink) CurrentExpression = InExpressionName;
	switch (InExpressionName)
	{
		case ETutorialBotExpression::HappyWink: MID_FaceExpression->SetScalarParameterValue("Expression", 0.f); break;
		case ETutorialBotExpression::NeutralWink: MID_FaceExpression->SetScalarParameterValue("Expression", 0.2f); break;
		case ETutorialBotExpression::Blink: MID_FaceExpression->SetScalarParameterValue("Expression", 0.3f); break;
		case ETutorialBotExpression::Thinking: MID_FaceExpression->SetScalarParameterValue("Expression", 0.4f); break;
		case ETutorialBotExpression::Sad: MID_FaceExpression->SetScalarParameterValue("Expression", 0.5f); break;
		case ETutorialBotExpression::Happy: MID_FaceExpression->SetScalarParameterValue("Expression", 0.7f); break;
		case ETutorialBotExpression::Grumpy: MID_FaceExpression->SetScalarParameterValue("Expression", 0.8f); break;
		case ETutorialBotExpression::NeutralFace: MID_FaceExpression->SetScalarParameterValue("Expression", 0.9f);
	}
}

void ATutorialBot::SetupNextRandomBlink()
{
	SetExpression(CurrentExpression);
	GetWorldTimerManager().SetTimer(RandomBlinkHandle, this, &ATutorialBot::TriggerBlink, FMath::FRandRange(MinRange, MaxRange), false);
}

void ATutorialBot::TriggerBlink()
{
	SetExpression(ETutorialBotExpression::Blink);
	GetWorldTimerManager().SetTimer(EndBlinkHandle, this, &ATutorialBot::SetupNextRandomBlink, 0.15f, false);
}

void ATutorialBot::SetupExpression()
{
	MID_FaceExpression = Mesh->CreateAndSetMaterialInstanceDynamic(1);
	SetExpression(ETutorialBotExpression::NeutralFace);
	SetupNextRandomBlink();
}

void ATutorialBot::EnableCheckDistanceToTarget()
{
	GetWorldTimerManager().SetTimer(CheckDistanceToTargetHandle, this, &ATutorialBot::CheckDistanceToTarget, 0.1f, true);
}

void ATutorialBot::CheckDistanceToTarget()
{
	if (!Player) return;
	if (FVector::DistSquared(GetActorLocation(), Player->GetActorLocation()) < 90000.f)
	{
		if (!bIsTutorialFinished) StartNextDialogue(bIsTutorialPressKeyToContinueNeeded);
		else StartTipDialogue();
		GetWorldTimerManager().ClearTimer(CheckDistanceToTargetHandle);
	}
}

void ATutorialBot::StartTipDialogue() const { Dialogue->StartTipDialogue(CurrentGameState); }

void ATutorialBot::PlayNextToGateAnimation()
{
	SetExpression(ETutorialBotExpression::Happy);
	SetupAndPlayNextAnimation(ETutorialBotAnimState::Wave, &ATutorialBot::PlayIdleAnimation);
}

void ATutorialBot::PlayIdleAnimation() { SetupAndPlayNextAnimation(ETutorialBotAnimState::Idle); }

void ATutorialBot::SpawnMarker() const
{
	GetWorld()->GetSubsystem<UMarkerManager>()->SpawnMarkerAtLocation(GetActorLocation() + FVector::UpVector * 200.f, false, EMarkerType::Exclamation);
}

void ATutorialBot::Interact()
{
	if (!GetIsInteractable()) return;
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UAbilitiesMenuUI>(EWidget::AbilitiesMenuUI)->OpenUI();
}

void ATutorialBot::SetupMeshOptimization() const
{
	Mesh->bEnableUpdateRateOptimizations = true;
	Mesh->SkipUpdateOverlapsOptimEnabled = true;
	Mesh->SetAllowAnimCurveEvaluation(false);
	Mesh->bUseRefPoseOnInitAnim = true;
	Mesh->SetComponentTickInterval(1.f / 30.f);
}

void ATutorialBot::SetArtifactCollected()
{
	if (bCurrentWaypointAssociatedWithSecretRoom) MoveToSecretRoomWaypoint();
	bArtifactCollected = true;
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnArtifactCollected.RemoveAll(this);
}

void ATutorialBot::MoveToSecretRoomWaypoint()
{
	for (int32 i = 0; i < Waypoints.Num(); i++) // Inefficient but since there will be max 5 waypoints does not matter.
	{
		if (Waypoints[i]->GetOrder() != -1) continue;
		SetActorLocationAndRotation(Waypoints[i]->GetActorLocation(), Waypoints[i]->GetActorRotation());
		CurrentGameState = EGameState::SecretRoom;
		Dialogue->SetNextDialogueText(CurrentGameState);
		break;
	}
}

void ATutorialBot::SetExpressionAndPlayAnimation(const ETutorialBotAnimState InAnimState, const ETutorialBotExpression InExpression)
{
	SetExpression(InExpression);
	SetupAndPlayNextAnimation(InAnimState);
}

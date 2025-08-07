#include "TutorialBot.h"
#include "TutorialBotAnimationInstance.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Navigation/DroneWaypoint.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Utilities/Global Utilities/UI Actors/TutorialDialogue.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

ATutorialBot::ATutorialBot()
{
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	TabletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TabletMesh"));
	TabletMesh->SetVisibility(false);
	Sounds.Add(ESound::BotHitVent, CreateDefaultSubobject<UAudioComponent>(TEXT("botHitVentSound")));
	Sounds[ESound::BotHitVent]->SetAutoActivate(false);
	Sounds[ESound::BotHitVent]->SetupAttachment(Mesh);
	Sounds.Add(ESound::BotBreakVent, CreateDefaultSubobject<UAudioComponent>(TEXT("botBreakVentSound")));
	Sounds[ESound::BotBreakVent]->SetAutoActivate(false);
	Sounds[ESound::BotBreakVent]->SetupAttachment(Mesh);
	Sounds.Add(ESound::BotLandOnHead, CreateDefaultSubobject<UAudioComponent>(TEXT("botLandOnHeadSound")));
	Sounds[ESound::BotLandOnHead]->SetAutoActivate(false);
	Sounds[ESound::BotLandOnHead]->SetupAttachment(Mesh);
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
	SetupExpression();
	TabletMesh->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "TutorialTabletSocket");
	RetrieveWaypoints();
	AnimInstance = Cast<UTutorialBotAnimationInstance>(Mesh->GetAnimInstance());
	GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->StartTutorialBotAnimation.BindUObject(this, &ATutorialBot::PlayNextAnimation);
	GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->OnShowPressKeyToContinue.AddUObject(this, &ATutorialBot::PressAnyKeyPressed);
	SetupDialogue();
	PlayFirstAnim();
}

void ATutorialBot::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearTimers();
	Super::EndPlay(EndPlayReason);
}

void ATutorialBot::PlaySound(const ESound InSoundName)
{
	Sounds[InSoundName]->Sound ? Sounds[InSoundName]->Play() : GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(Sounds[InSoundName], nullptr, InSoundName);
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
	GetWorld()->GetTimerManager().ClearTimer(MoveToLocationHandle);
	GetWorld()->GetTimerManager().ClearTimer(ResetControlRigStateHandle);
	GetWorld()->GetTimerManager().ClearTimer(TabletVisibilityHandle);
	GetWorld()->GetTimerManager().ClearTimer(RandomBlinkHandle);
	GetWorld()->GetTimerManager().ClearTimer(EndBlinkHandle);
	
}

void ATutorialBot::SetupDialogue()
{
	Dialogue = GetWorld()->SpawnActor<ATutorialDialogue>(DialogueClass);
	Dialogue->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "BehindHeadSocket");
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
	GetWorld()->GetTimerManager().SetTimer(AnimFinishedHandle, this, InCallback, InTimer > 0.f ? InTimer : Animation->GetPlayLength());
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
	(void)GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->OnShowHint.ExecuteIfBound();
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->SpawnTutorialEnemy.ExecuteIfBound(ECharacterType::Guardian, EGameMode::TutorialMode1);
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

void ATutorialBot::SetupAndMoveToNextLocation(const bool bInstantMove)
{
	Dialogue->SetVisibilityState(false);
	for (int32 i = 0; i < Waypoints.Num(); i++) if (Waypoints[i]->GetOrder() == CurrentWaypointIndex) WaypointLocation = Waypoints[i]->GetActorLocation();
	CurrentWaypointIndex++;
	if (bInstantMove)
	{
		SetActorLocation(WaypointLocation);
		return;
	}
	bIsMoving = true;
	GetWorldTimerManager().SetTimer(MoveToLocationHandle, this, &ATutorialBot::MoveToLocation, 0.015f, true);
}

void ATutorialBot::MoveToLocation()
{
	const FVector Direction = (WaypointLocation - GetActorLocation()).GetSafeNormal();
	const FVector NewLocation = GetActorLocation() + Direction * MovementSpeed;
	SetActorLocation(NewLocation);
	if (FVector::DistSquared(GetActorLocation(), WaypointLocation) < FMath::Square(2.f))
	{
		bIsMoving = false;
		GetWorldTimerManager().ClearTimer(MoveToLocationHandle);
		switch (CurrentWaypointIndex)
		{
			case 1: SetupAndMoveToNextLocation(); break;
			case 2:
				SetControlRigState(false);
				SetupAndPlayNextAnimation(ETutorialBotAnimState::Tutorial13, &ATutorialBot::ThirteenthAnimationFinished);
				break;
			default: ;
		}
	}
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
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnAlertStart.Broadcast();
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnMusicSoundtrackChangeRequired.ExecuteIfBound();
}

void ATutorialBot::ThirteenthAnimationFinished()
{
	StartNextDialogue();
	SetupAndPlayNextAnimation(ETutorialBotAnimState::Tutorial14, &ATutorialBot::FourteenthAnimationFinished);
}

void ATutorialBot::FourteenthAnimationFinished()
{
	SetControlRigState(true);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnShowPressKeyToContinue.Broadcast(true);
}

void ATutorialBot::PressAnyKeyPressed(const bool bState)
{
	if (bState) return;
	switch (PressKeyIndex)
	{
		case 0: case 1: StartNextDialogue(true); break;
		case 2:
			StartNextDialogue();
			(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnShowHint.ExecuteIfBound();
			(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->EnableMeleeChecking.ExecuteIfBound();
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

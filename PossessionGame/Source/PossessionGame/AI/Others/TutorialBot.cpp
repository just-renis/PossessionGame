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

TObjectPtr<UAnimMontage> ATutorialBot::GetAnimation(const ETutorialBotAnimState InAnimState)
{
	if (const TObjectPtr<UAnimMontage> Animation = *Animations.Find(InAnimState)) return Animation;
	return nullptr;
}

void ATutorialBot::BeginPlay()
{
	Super::BeginPlay();
	RetrieveWaypoints();
	AnimInstance = Cast<UTutorialBotAnimationInstance>(Mesh->GetAnimInstance());
	GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->StartTutorialBotAnimation.BindUObject(this, &ATutorialBot::PlayNextAnimation);
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
	GetWorld()->GetTimerManager().SetTimer(HitVent1Handle, [this] { PlaySound(ESound::BotHitVent); }, 8.62f, false);
	GetWorld()->GetTimerManager().SetTimer(HitVent2Handle, [this] { PlaySound(ESound::BotHitVent); }, 9.59f, false);
	GetWorld()->GetTimerManager().SetTimer(HitVent3Handle, [this] { PlaySound(ESound::BotHitVent); }, 10.42f, false);
	GetWorld()->GetTimerManager().SetTimer(BreakVentHandle, [this]{ PlaySound(ESound::BotBreakVent); }, 11.69f, false);
	GetWorld()->GetTimerManager().SetTimer(LandOnHeadHandle, [this] { PlaySound(ESound::BotLandOnHead); }, 11.85f, false);
}

void ATutorialBot::ClearTimers()
{
	GetWorld()->GetTimerManager().ClearTimer(HitVent1Handle);
	GetWorld()->GetTimerManager().ClearTimer(HitVent2Handle);
	GetWorld()->GetTimerManager().ClearTimer(HitVent3Handle);
	GetWorld()->GetTimerManager().ClearTimer(BreakVentHandle);
	GetWorld()->GetTimerManager().ClearTimer(LandOnHeadHandle);
}

void ATutorialBot::SetupDialogue()
{
	Dialogue = GetWorld()->SpawnActor<ATutorialDialogue>(DialogueClass);
	Dialogue->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "BehindHeadSocket");
	Dialogue->SetVisibilityState(false);
}

void ATutorialBot::StartNextDialogue()
{
	Dialogue->SetVisibilityState(true);
	Dialogue->StartDialogue(TutorialDialogueIndex++);
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
	Dialogue->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "OverHeadSocket");
	SetupAndPlayNextAnimation(ETutorialBotAnimState::Tutorial3, &ATutorialBot::ThirdAnimationFinished);
}

void ATutorialBot::ThirdAnimationFinished()
{
	StartNextDialogue();
	SetupAndPlayNextAnimation(ETutorialBotAnimState::Tutorial4, &ATutorialBot::FourthAnimationFinished);
}

void ATutorialBot::FourthAnimationFinished()
{
	Dialogue->SetVisibilityState(false);
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
}

void ATutorialBot::SeventhAnimationFinished()
{
	StartNextDialogue();
	SetupAndPlayNextAnimation(ETutorialBotAnimState::Tutorial8, &ATutorialBot::EighthAnimationFinished);
}

void ATutorialBot::EighthAnimationFinished()
{
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->SpawnTutorialEnemy.ExecuteIfBound(ECharacterType::Guardian, EGameMode::TutorialMode1);
}

void ATutorialBot::PlayNextAnimation()
{
	switch (CurrentTutorialAnimation)
	{
		case 8:
			SetControlRigState(true);
			StartNextDialogue();
			SetupAndPlayNextAnimation(ETutorialBotAnimState::Tutorial9, &ATutorialBot::NinthAnimationFinished, 2.5f);
		default: ;
	}
}

void ATutorialBot::NinthAnimationFinished()
{
	SetControlRigState(false);
	SetupAndPlayNextAnimation(ETutorialBotAnimState::Tutorial10, &ATutorialBot::TenthAnimationFinished);
}

void ATutorialBot::TenthAnimationFinished()
{
	SetControlRigState(true);
	StartNextDialogue();
	SetupAndPlayNextAnimation(ETutorialBotAnimState::Tutorial11, &ATutorialBot::EleventhAnimationFinished);
}

void ATutorialBot::EleventhAnimationFinished()
{
	AnimInstance->SetIsControlRigEnabled(false);
	SetupAndPlayNextAnimation(ETutorialBotAnimState::Tutorial12, &ATutorialBot::TwelfthAnimationFinished);
	SetupAndMoveToNextLocation();
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
	FVector Direction = (WaypointLocation - GetActorLocation()).GetSafeNormal();
	Direction.Z = 0;
	const FVector NewLocation = GetActorLocation() + Direction * MovementSpeed;
	SetActorLocation(NewLocation);
	if (FVector::DistSquared(GetActorLocation(), WaypointLocation) < FMath::Square(10.f))
	{
		bIsMoving = false;
		GetWorldTimerManager().ClearTimer(MoveToLocationHandle);
		switch (CurrentWaypointIndex)
		{
			case 0: SetupAndMoveToNextLocation(); break;
			default: ;
		}
	}
}

void ATutorialBot::TwelfthAnimationFinished()
{
	AnimInstance->SetIsControlRigEnabled(true);
}

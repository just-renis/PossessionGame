#include "TutorialBot.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Utilities/Global Utilities/UI Actors/TutorialDialogue.h"
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

TObjectPtr<UAnimationAsset> ATutorialBot::GetAnimation(const ETutorialBotAnimState InAnimState)
{
	if (const TObjectPtr<UAnimationAsset> Animation = *Animations.Find(InAnimState)) return Animation;
	return nullptr;
}

void ATutorialBot::BeginPlay()
{
	Super::BeginPlay();
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

void ATutorialBot::SetupAndPlayNextAnimation(const ETutorialBotAnimState InAnimState, const FAnimCallback InCallback)
{
	const TObjectPtr<UAnimationAsset> Animation = GetAnimation(InAnimState);
	GetWorld()->GetTimerManager().SetTimer(AnimFinishedHandle, this, InCallback, Animation->GetPlayLength(), false);
	Mesh->PlayAnimation(Animation, false);
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
	
}

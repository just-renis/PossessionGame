#include "SecretCrate.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/UI/MainWidgets/SecretRoomUI.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

ASecretCrate::ASecretCrate()
{
	CrateMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("crateMesh"));
	CrateMesh->SetupAttachment(RootComponent);
	Sounds.Add(ESound::SecretCratePassive, CreateDefaultSubobject<UAudioComponent>(TEXT("passiveSound")));
	Sounds[ESound::SecretCratePassive]->SetAutoActivate(false);
	Sounds[ESound::SecretCratePassive]->SetupAttachment(CrateMesh);
	Sounds.Add(ESound::SecretCrateOpen, CreateDefaultSubobject<UAudioComponent>(TEXT("openSound")));
	Sounds[ESound::SecretCrateOpen]->SetAutoActivate(false);
	Sounds[ESound::SecretCrateOpen]->SetupAttachment(CrateMesh);
	Sounds.Add(ESound::SecretCrateReceivePowerUp, CreateDefaultSubobject<UAudioComponent>(TEXT("receivePowerUpSound")));
	Sounds[ESound::SecretCrateReceivePowerUp]->SetAutoActivate(false);
	Sounds[ESound::SecretCrateReceivePowerUp]->SetupAttachment(CrateMesh);
}

void ASecretCrate::BeginPlay()
{
	Super::BeginPlay();
	PlaySound(ESound::SecretCratePassive);
}

void ASecretCrate::Interact()
{
	if (!GetIsInteractable()) return;
	StopSound(ESound::SecretCratePassive);
	PlaySound(ESound::SecretCrateOpen);
	CrateMesh->PlayAnimation(OpenAnim, false);
	Collision->OnComponentBeginOverlap.RemoveAll(this);
	Collision->OnComponentEndOverlap.RemoveAll(this);
	FadeOutInteractableUI();
	SetIsInteractable(false);
	GiveReward();
}

void ASecretCrate::GiveReward()
{
	PlaySound(ESound::SecretCrateReceivePowerUp);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->SpawnVfx(EVfx::SecretRewardPopUp, nullptr, GetActorLocation() + FVector(0.f, 0.f, 30.f));
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<USecretRoomUI>(EWidget::SecretRoomUI)->OpenUI();
}

void ASecretCrate::PlaySound(const ESound InSoundName)
{
	Sounds[InSoundName]->Sound ? Sounds[InSoundName]->Play() : GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(Sounds[InSoundName], nullptr, InSoundName);
}

void ASecretCrate::StopSound(const ESound InSoundName)
{
	if (!Sounds.Contains(InSoundName)) return;
	Sounds[InSoundName]->Stop();
}

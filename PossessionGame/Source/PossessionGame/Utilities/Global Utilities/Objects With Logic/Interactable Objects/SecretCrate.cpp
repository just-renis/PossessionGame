#include "SecretCrate.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

ASecretCrate::ASecretCrate()
{
	CrateMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("crateMesh"));
	RootComponent = CrateMesh;
	Collision->SetupAttachment(RootComponent);
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
	SetIsInteractable(true);
}

void ASecretCrate::Interact()
{
	if (!GetIsInteractable()) return;
	StopSound(ESound::SecretCratePassive);
	PlaySound(ESound::SecretCrateOpen);
	CrateMesh->PlayAnimation(OpenAnim, false);
	Collision->OnComponentBeginOverlap.RemoveAll(this);
	Collision->OnComponentEndOverlap.RemoveAll(this);
	if (InteractableWidget) InteractableWidget->SetVisibility(false);
	SetIsInteractable(false);
	GiveReward();
}

void ASecretCrate::GiveReward()
{
	PlaySound(ESound::SecretCrateReceivePowerUp);
	const int32 RandomIndex = FMath::RandRange(0, RewardMaterials.Num() - 1);
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnSecretRewardCollected.ExecuteIfBound(RandomIndex);
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->SpawnVfx(EVfx::SecretRewardPopUp, nullptr, GetActorLocation(), FRotator::ZeroRotator,
	FNiagaraVariationData(), {{"Material", FNiagaraAttribute(true, RewardMaterials[RandomIndex])}});
}

void ASecretCrate::PlaySound(const ESound InSoundName)
{
	Sounds[InSoundName]->Sound ? Sounds[InSoundName]->Play() : GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(Sounds[InSoundName], InSoundName);
}

void ASecretCrate::StopSound(const ESound InSoundName)
{
	if (!Sounds.Contains(InSoundName)) return;
	Sounds[InSoundName]->Stop();
}

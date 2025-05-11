#include "SecretCrate.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

ASecretCrate::ASecretCrate()
{
	RewardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	CrateMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("crateMesh"));
	RootComponent = CrateMesh;
	Collision->SetupAttachment(RootComponent);
	RewardMesh->SetupAttachment(RootComponent);
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
	RewardMesh->SetVisibility(false);
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
	UKismetSystemLibrary::K2_SetTimer(this, "RewardMeshMoveUpwards", 0.025f, true);
	UKismetSystemLibrary::K2_SetTimer(this, "StopRewardMesh", 3.f, false);
}

void ASecretCrate::GiveReward()
{
	PlaySound(ESound::SecretCrateReceivePowerUp);
	const int32 RandomIndex = FMath::RandRange(0, RewardMaterials.Num() - 1);
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnSecretRewardCollected.ExecuteIfBound(RandomIndex);
	RewardMesh->SetMaterial(0, RewardMaterials[RandomIndex]);
	RewardMesh->SetVisibility(true);
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

void ASecretCrate::RewardMeshMoveUpwards() const
{
	RewardMesh->SetRelativeLocation(RewardMesh->GetRelativeLocation() + FVector(0.f, 0.f, 0.5f));
}

void ASecretCrate::StopRewardMesh()
{
	UKismetSystemLibrary::K2_ClearTimer(this, "RewardMeshMoveUpwards");
	RewardMesh->SetVisibility(false);
}

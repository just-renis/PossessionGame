#include "SecretCrate.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "PossessionGame/UI/SecondaryWidgets/CustomTextUI.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

ASecretCrate::ASecretCrate()
{
	CrateMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("crateMesh"));
	CrateMesh->SetupAttachment(RootComponent);
	CustomTextWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("customTextWidgetComponent"));
	CustomTextWidget->SetupAttachment(RootComponent);
	CustomTextWidget->SetVisibility(false);
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
	FadeOutInteractableUI();
	SetIsInteractable(false);
	GiveReward();
}

void ASecretCrate::GiveReward()
{
	PlaySound(ESound::SecretCrateReceivePowerUp);
	const int32 RandomIndex = FMath::RandRange(0, RewardMaterials.Num() - 1);
	SetupRewardText(RandomIndex);
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnSecretRewardCollected.ExecuteIfBound(RandomIndex);
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->SpawnVfx(EVfx::SecretRewardPopUp, nullptr, GetActorLocation() + FVector(0.f, 0.f, 30.f), FRotator::ZeroRotator,
	FNiagaraVariationData(), {{"Material", FNiagaraAttribute(true, RewardMaterials[RandomIndex])}});
}

void ASecretCrate::PlaySound(const ESound InSoundName)
{
	Sounds[InSoundName]->Sound ? Sounds[InSoundName]->Play() : GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(Sounds[InSoundName], nullptr, InSoundName);
}

void ASecretCrate::StopSound(const ESound InSoundName)
{
	if (!Sounds.Contains(InSoundName)) return;
	Sounds[InSoundName]->Stop();
}

void ASecretCrate::SetupRewardText(const int32 InIndex) const
{
	const TObjectPtr<UCustomTextUI> CustomTextUI = Cast<UCustomTextUI>(CustomTextWidget->GetWidget());
	switch (InIndex)
	{
		case 0: CustomTextUI->SetText(FText::FromString("HP Restored")); break;
		case 1: CustomTextUI->SetText(FText::FromString("Hijack Charge Restored")); break;
		case 2: CustomTextUI->SetText(FText::FromString("XP Gained"));
		default: ;
	}
	CustomTextWidget->SetVisibility(true);
	CustomTextUI->PlayFadeInAnim(true);
}

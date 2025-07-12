#include "SecretCrate.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PostProcessManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

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

EVariation ASecretCrate::SetupReward(const int32 InIndex)
{
	EVariation VariationName = {};
	switch (InIndex)
	{
		case 0:
			DelegateManager->RequirePlayerAnnouncementInfo.ExecuteIfBound(FText::FromString("Health Restored"));
			if (const TObjectPtr<UMaterialInterface>* Material = PP_Materials.Find(ERewardMaterial::Health)) PP_Material = UMaterialInstanceDynamic::Create(*Material, this);
			VariationName = EVariation::SecretCrateRewardHealth;
			break;
		case 1:
			DelegateManager->RequirePlayerAnnouncementInfo.ExecuteIfBound(FText::FromString("Hijack Restored"));
			if (const TObjectPtr<UMaterialInterface>* Material = PP_Materials.Find(ERewardMaterial::Hijack)) PP_Material = UMaterialInstanceDynamic::Create(*Material, this);
			VariationName = EVariation::SecretCrateRewardHijack;
			break;
		case 2:
			DelegateManager->RequirePlayerAnnouncementInfo.ExecuteIfBound(FText::FromString("XP Gained"));
			if (const TObjectPtr<UMaterialInterface>* Material = PP_Materials.Find(ERewardMaterial::Xp)) PP_Material = UMaterialInstanceDynamic::Create(*Material, this);
			VariationName = EVariation::SecretCrateRewardXp;
		default: ;
	}
	GetWorld()->GetSubsystem<UPostProcessManager>()->AddOrUpdateDynamicMaterial(EPostProcessMaterial::SecretCrateReward, PP_Material, 1.f);
	GetWorldTimerManager().SetTimer(ProcessRewardHandle, this, &ASecretCrate::ProcessRewardAppear, 0.0125f, true);
	return VariationName;
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
	const int32 RandomIndex = FMath::RandRange(0, RewardMaterials.Num() - 1);
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnSecretRewardCollected.ExecuteIfBound(RandomIndex);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->SpawnVfx(EVfx::SecretRewardPopUp, nullptr, GetActorLocation() + FVector(0.f, 0.f, 30.f), FRotator::ZeroRotator, SetupReward(RandomIndex));
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

void ASecretCrate::ProcessRewardAppear()
{
	ProcessRewardLoopValue++;
	if (!PP_Material) return;
	PP_Material->SetScalarParameterValue("Appear", 0.05f * ProcessRewardLoopValue);
	PP_Material->SetScalarParameterValue("RadialDensity", 3 - 0.09f * ProcessRewardLoopValue);
	if (ProcessRewardLoopValue >= 20)
	{
		ProcessRewardLoopValue = 0;
		GetWorldTimerManager().ClearTimer(ProcessRewardHandle);
		GetWorldTimerManager().SetTimer(ProcessRewardHandle, this, &ASecretCrate::ProcessRewardDisappear, 0.0125f, true, 1.f);
	}
}

void ASecretCrate::ProcessRewardDisappear()
{
	ProcessRewardLoopValue++;
	if (!PP_Material) return;
	PP_Material->SetScalarParameterValue("Appear", 1.f - 0.05f * ProcessRewardLoopValue);
	PP_Material->SetScalarParameterValue("RadialDensity", 1.8f + 0.09f * ProcessRewardLoopValue);
	if (ProcessRewardLoopValue >= 20)
	{
		ProcessRewardLoopValue = 0;
		GetWorldTimerManager().ClearTimer(ProcessRewardHandle);
	}
}

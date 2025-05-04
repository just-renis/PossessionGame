#include "SecretCrate.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

ASecretCrate::ASecretCrate()
{
	RewardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	CrateMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("crateMesh"));
	RootComponent = CrateMesh;
	RewardMesh->SetupAttachment(RootComponent);
}

void ASecretCrate::BeginPlay()
{
	Super::BeginPlay();
	RewardMesh->SetVisibility(false);
	SetIsInteractable(true);
}

void ASecretCrate::Interact()
{
	if (!GetIsInteractable()) return;
	CrateMesh->PlayAnimation(OpenAnim, false);
	Collision->OnComponentBeginOverlap.RemoveAll(this);
	Collision->OnComponentEndOverlap.RemoveAll(this);
	if (InteractableWidget) InteractableWidget->SetVisibility(false);
	SetIsInteractable(false);
	GiveReward();
	UKismetSystemLibrary::K2_SetTimer(this, "RewardMeshMoveUpwards", 0.05f, true);
	UKismetSystemLibrary::K2_SetTimer(this, "StopRewardMesh", 3.f, false);
}

void ASecretCrate::GiveReward()
{
	const int32 RandomIndex = FMath::RandRange(0, RewardMaterials.Num() - 1);
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnSecretRewardCollected.ExecuteIfBound(RandomIndex);
	RewardMesh->SetMaterial(0, RewardMaterials[RandomIndex]);
	RewardMesh->SetVisibility(true);
}

void ASecretCrate::RewardMeshMoveUpwards() const
{
	RewardMesh->SetRelativeLocation(RewardMesh->GetRelativeLocation() + FVector(0.f, 0.f, 1.f));
}

void ASecretCrate::StopRewardMesh()
{
	UKismetSystemLibrary::K2_ClearTimer(this, "RewardMeshMoveUpwards");
	RewardMesh->SetVisibility(false);
}

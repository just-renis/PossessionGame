#include "SecretCrate.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

ASecretCrate::ASecretCrate()
{
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("collision"));
	RewardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("rewardMesh"));
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("mesh"));
	InteractableWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("interactableWidgetComponent"));
	RootComponent = Mesh;
	Collision->SetupAttachment(Mesh);
	RewardMesh->SetupAttachment(Mesh);
	InteractableWidget->SetupAttachment(RootComponent);
}

void ASecretCrate::BeginPlay()
{
	Super::BeginPlay();
	RewardMesh->SetVisibility(false);
	if (InteractableWidget) InteractableWidget->SetVisibility(false);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ASecretCrate::EnableInteractionWidget);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ASecretCrate::DisableInteractionWidget);
	SetIsInteractable(true);
}

void ASecretCrate::Interact()
{
	if (!GetIsInteractable()) return;
	Mesh->PlayAnimation(OpenAnim, false);
	Collision->OnComponentBeginOverlap.RemoveAll(this);
	Collision->OnComponentEndOverlap.RemoveAll(this);
	if (InteractableWidget) InteractableWidget->SetVisibility(false);
	SetIsInteractable(false);
	GiveReward();
	UKismetSystemLibrary::K2_SetTimer(this, "RewardMeshMoveUpwards", 0.05f, true);
	UKismetSystemLibrary::K2_SetTimer(this, "StopRewardMesh", 3.f, false);
}

void ASecretCrate::ChangeOutlineState(const bool bState)
{
	Mesh->SetOverlayMaterial(bState ? MI_InteractionOutline : nullptr);
	if (bState) UKismetSystemLibrary::K2_SetTimer(this, "DisableOutline", 0.245f, false);
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

void ASecretCrate::EnableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Player")) InteractableWidget->SetVisibility(true);
}

void ASecretCrate::DisableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag("Player")) InteractableWidget->SetVisibility(false);
}

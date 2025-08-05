#include "TutorialButton.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

ATutorialButton::ATutorialButton()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	MeshFrame = CreateDefaultSubobject<UStaticMeshComponent>("MeshFrame");
	Collision = CreateDefaultSubobject<USphereComponent>("Collision");
	Collision->SetCollisionProfileName("TriggerPlayer");
	RootComponent = MeshFrame;
	Mesh->SetupAttachment(RootComponent);
	Collision->SetupAttachment(RootComponent);
}

void ATutorialButton::BeginPlay()
{
	Super::BeginPlay();
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ATutorialButton::OverlapOccured);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ATutorialButton::OverlapEnded);
}

void ATutorialButton::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(CheckIfStompedHandle);
	Super::EndPlay(EndPlayReason);
}

void ATutorialButton::ButtonClicked()
{
	GetWorldTimerManager().ClearTimer(CheckIfStompedHandle);
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetRelativeLocation(Mesh->GetRelativeLocation() - FVector(0.f, 0.f, 20.f));
	GetGameInstance()->GetSubsystem<UDelegateManager>()->StartForceFieldDissolve.Broadcast(ButtonIndex);
}

void ATutorialButton::StompCheck()
{
	if (!OverlappedPlayer) return;
	if (OverlappedPlayer->GetIsStomping()) ButtonClicked();
}

void ATutorialButton::OverlapOccured(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player")) return;
	if (const TObjectPtr<AEnemy> Player = Cast<AEnemy>(OtherActor))
	{
		OverlappedPlayer = Player;
		GetWorldTimerManager().SetTimer(CheckIfStompedHandle, this, &ATutorialButton::StompCheck, 0.01f, true);
	}
}

void ATutorialButton::OverlapEnded(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor->ActorHasTag("Player")) return;
	OverlappedPlayer = nullptr;
	GetWorldTimerManager().ClearTimer(CheckIfStompedHandle);
}

#include "TutorialButton.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/MarkerManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

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
	if (bTerminalButton)
	{
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTerminalCandleInteraction.AddUObject(this, &ATutorialButton::DisableActiveStateWhenOtherButtonStomped);
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTerminalHackFinish.AddUObject(this, &ATutorialButton::TerminalHijackFinished);
	}
	if (!GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetIsTutorialFinished()) GetGameInstance()->GetSubsystem<UDelegateManager>()->SpawnMarkerOnTutorialButton.BindUObject(this, &ATutorialButton::SpawnMarker);
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
	if (bTerminalButton) GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTerminalCandleInteraction.Broadcast(ButtonIndex);
	else GetGameInstance()->GetSubsystem<UDelegateManager>()->StartForceFieldDissolve.Broadcast(ButtonIndex);
	if (!GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetIsTutorialFinished()) GetWorld()->GetSubsystem<UMarkerManager>()->GetTriggerDestroyMarkersDelegate().Broadcast();
}

void ATutorialButton::StompCheck()
{
	if (!OverlappedPlayer) return;
	if (OverlappedPlayer->GetIsStomping()) ButtonClicked();
}

void ATutorialButton::SpawnMarker() const { GetWorld()->GetSubsystem<UMarkerManager>()->SpawnMarkerAtLocation(GetActorLocation() + FVector::UpVector * 70.f, false); }

void ATutorialButton::DisableActiveStateWhenOtherButtonStomped(const int32 InClickedButtonId) { bActive = false; }

void ATutorialButton::TerminalHijackFinished(const EGameMode InGameMode) { bActive = true; }

void ATutorialButton::OverlapOccured(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player") || !bActive) return;
	if (const TObjectPtr<AEnemy> Player = Cast<AEnemy>(OtherActor))
	{
		OverlappedPlayer = Player;
		GetWorldTimerManager().SetTimer(CheckIfStompedHandle, this, &ATutorialButton::StompCheck, 0.01f, true);
	}
}

void ATutorialButton::OverlapEnded(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor->ActorHasTag("Player") || !bActive) return;
	OverlappedPlayer = nullptr;
	GetWorldTimerManager().ClearTimer(CheckIfStompedHandle);
}

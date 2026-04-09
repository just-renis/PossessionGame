#include "Terminal.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/TutorialObjects/TutorialButton.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/MarkerManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

ATerminal::ATerminal()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	Mesh->SetupAttachment(RootComponent);
}

void ATerminal::BeginPlay()
{
	Super::BeginPlay();
	DelegateManager->OnHackModeStart.AddUObject(this, &ATerminal::ShowMarker);
	if (bIsButton)
	{
		DelegateManager->OnTerminalCandleInteraction.AddUObject(this, &ATerminal::InteractWithButtonId);
		DelegateManager->OnInteractionTry.RemoveAll(this);
		DelegateManager->OnTerminalHackFinish.AddUObject(this, &ATerminal::EnableTerminal);
		SetIsInteractable(true);
	}
	else
	{
		DelegateManager->OnTerminalInteraction.AddUObject(this, &ATerminal::DisableTerminal);
		DelegateManager->OnTerminalHackFinish.AddUObject(this, &ATerminal::EnableTerminal);
		DelegateManager->OnGameModeInitiator.AddUObject(this, &ATerminal::EnableTerminal);
	}
}

void ATerminal::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(ValveRotationHandle);
	Super::EndPlay(EndPlayReason);
}

void ATerminal::Interact()
{
	if (!GetIsInteractable()) return;
	if (bIsValve) GetWorldTimerManager().SetTimer(ValveRotationHandle, this, &ATerminal::ProcessRotationOfValve, 1.f / 36.f, true);
	DelegateManager->SendHackTerminalRoomMode.Broadcast(RoomMode == EGameMode::None ? EGameMode::HackTerminalsMode : RoomMode);
	GetWorld()->GetSubsystem<UMarkerManager>()->DestroyMarkers();
	DelegateManager->OnTerminalInteraction.RemoveAll(this);
	DelegateManager->OnTerminalHackFinish.RemoveAll(this);
	DelegateManager->OnGameModeInitiator.RemoveAll(this);
	DisableTerminal();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTerminalInteraction.Broadcast();
}

void ATerminal::SetupInteractionLogic() { if (!bIsButton) Super::SetupInteractionLogic(); }

void ATerminal::ShowMarker() const { GetWorld()->GetSubsystem<UMarkerManager>()->SpawnMarkerAtLocation(GetActorLocation() + FVector::UpVector * 140.f); }

void ATerminal::EnableTerminal(const EGameMode InGameMode)
{
	if (InGameMode == EGameMode::HackTerminalsMode) ToggleInteractableWidget(true);
}

void ATerminal::EnableButtonTerminal(const EGameMode InGameMode)
{
	if (InGameMode == EGameMode::HackTerminalsMode) ShowMarker();
}

void ATerminal::DisableTerminal()
{
	ToggleInteractableWidget(false);
	SetIsInteractable(false);
}

void ATerminal::ToggleInteractableWidget(const bool bState)
{
	if (bState)
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		ShowMarker();
	}
	else
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		FadeOutInteractableUI();
	}
}

void ATerminal::ProcessRotationOfValve()
{
	Mesh->AddLocalRotation(FRotator(0.f, 10.f, 0.f));
	if (++RotationTimes < 36) return;
	GetWorldTimerManager().ClearTimer(ValveRotationHandle);
	const TObjectPtr<UPrimaryDataAssetsManager> AssetsManager = GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>();
	AssetsManager->PlaySoundAtLocation(ESound::HackTerminalSmoke, GetActorLocation() + SmokePipeOffset);
	AssetsManager->SpawnVfx(EVfx::GeneratorRoomSmoke, nullptr, GetActorLocation() + RoomSmokeOffset);
	AssetsManager->SpawnVfx(EVfx::GeneratorRoomPipeSmoke, nullptr, GetActorLocation() + SmokePipeOffset);
}

void ATerminal::InteractWithButtonId(const int32 InButtonId)
{
	if (Button->GetButtonIndex() != InButtonId) return;
	Interact();
}

#include "PlayerGhost.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "InputMappingContext.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/AI/Others/TutorialDrone.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"
#include "PossessionGame/UI/MainWidgets/PauseMenuUI.h"
#include "PossessionGame/UI/MainWidgets/PlayerGhostUI.h"
#include "PossessionGame/UI/MainWidgets/SettingsUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

APlayerGhost::APlayerGhost()
{
	PrimaryActorTick.bCanEverTick = true;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("camera"));
	Camera->SetupAttachment(RootComponent);
}

void APlayerGhost::BeginPlay()
{
	Super::BeginPlay();
	if (!Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot()->GetIsTutorialFinished()) SetupTutorial();
	Tags.Add("PlayerGhost");
	UKismetSystemLibrary::K2_SetTimer(this, "CheckIfPlayerFallOutOfWorld", 2.f, true);
	if (const TObjectPtr<APlayerController> PlayerController = Cast<APlayerController>(Controller))
	{
		if (!Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot()->GetIsTutorialFinished())
		{
			PlayerController->SetInputMode(FInputModeUIOnly());
			PlayerGhostUI->PlayStartupAnimation();
			UKismetSystemLibrary::K2_SetTimer(this, "EnablePlayerInput", PlayerGhostUI->GetStartupAnimationLength(), false);
		}
		if (const TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}
}

void APlayerGhost::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(0).Action, ETriggerEvent::Triggered, this, &APlayerGhost::Move);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(4).Action, ETriggerEvent::Triggered, this, &APlayerGhost::Jump);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(5).Action, ETriggerEvent::Triggered, this, &APlayerGhost::Rotate);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(6).Action, ETriggerEvent::Started, this, &APlayerGhost::StartRunning);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(6).Action, ETriggerEvent::Completed, this, &APlayerGhost::StopRunning);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(7).Action, ETriggerEvent::Triggered, this, &APlayerGhost::PossessEnemy);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(16).Action, ETriggerEvent::Started, this, &APlayerGhost::HandleEscapeClick);
	}
}

void APlayerGhost::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UKismetSystemLibrary::K2_ClearTimer(this, "CheckIfPlayerFallOutOfWorld");
	if (!PlayerGhostUI) return;
 	PlayerGhostUI->RemoveFromParent();
	PlayerGhostUI = nullptr;
}

void APlayerGhost::Move(const FInputActionValue& InValue)
{
	if (!Controller) return;
	const FVector2D MovementVector = InValue.Get<FVector2D>();
	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	AddMovementInput(GetActorRightVector(), MovementVector.X);
	if (CurrentTutorialStep == 0 && Target)
	{
		if (FVector::DistSquared(GetActorLocation(), Target->GetActorLocation()) < 20000.f)
		{
			DelegateManager->OnShowDialogue.Broadcast(0);
			CompleteTutorialStep();
		}
	}
}

void APlayerGhost::Rotate(const FInputActionValue& InValue)
{
	if (!Controller) return;
	const FVector2D RotationVector = InValue.Get<FVector2D>();
	AddControllerYawInput(RotationVector.X);
	FRotator NewRotation = Camera->GetComponentRotation();
	NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + RotationVector.Y, -89.f, 89.f);
	Camera->SetWorldRotation(NewRotation);
}

void APlayerGhost::StartRunning() {  GetCharacterMovement()->MaxWalkSpeed = 900; }

void APlayerGhost::StopRunning() {  GetCharacterMovement()->MaxWalkSpeed = 600; }

void APlayerGhost::PossessEnemy()
{
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	const FVector StartLocation = Camera->GetComponentLocation();
	QueryParams.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, StartLocation + Camera->GetForwardVector() * 15000.f, ECC_Visibility, QueryParams))
	{
		const TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(HitResult.GetActor());
		if (Enemy && Enemy->GetIsPossessable())
		{
			Controller->Possess(Enemy);
			Destroy();
		}
	}
}

void APlayerGhost::RestartLevel() const { UGameplayStatics::OpenLevel(GetWorld(), "/Game/Levels/Chapter1/Levels/Level_1"); }

void APlayerGhost::HandleEscapeClick()
{
	switch (GetWorld()->GetSubsystem<UWidgetManager>()->GetCurrentStateOfUI())
	{
		case EWidget::PauseMenuUI: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPauseMenuUI>(EWidget::PauseMenuUI)->CloseUI(); break;
		case EWidget::SettingsUI: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<USettingsUI>(EWidget::SettingsUI)->CloseUI(); break;
		default: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPauseMenuUI>(EWidget::PauseMenuUI)->OpenUI();
	}
}

void APlayerGhost::SetupTutorial()
{
	Target = UGameplayStatics::GetActorOfClass(GetWorld(), TutorialDroneClass);
	DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	DelegateManager->OnShowHint.AddUObject(this, &APlayerGhost::SetCurrentTutorialStep);
	PlayerGhostUI = CreateWidget<UPlayerGhostUI>(GetWorld(), PlayerGhostUIClass);
	PlayerGhostUI->AddToViewport();
}

void APlayerGhost::CheckIfPlayerFallOutOfWorld() const
{
	if (GetActorLocation().Z <= -1000.f)
	{
		if (GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->IsAsyncLoadingFinished()) RestartLevel();
	}
}

void APlayerGhost::EnablePlayerInput() const
{
	if (!Controller) return;
	if (const TObjectPtr<APlayerController> PlayerController = Cast<APlayerController>(Controller))
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		DelegateManager->OnShowHint.Broadcast(0);
	}
}

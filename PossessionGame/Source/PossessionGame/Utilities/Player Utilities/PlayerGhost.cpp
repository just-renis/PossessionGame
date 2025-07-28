#include "PlayerGhost.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "InputMappingContext.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"
#include "PossessionGame/UI/MainWidgets/PauseMenuUI.h"
#include "PossessionGame/UI/MainWidgets/PlayerGhostUI.h"
#include "PossessionGame/UI/MainWidgets/SettingsUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/GlobalSettingsManager.h"
#include "UserSettings/EnhancedInputUserSettings.h"

APlayerGhost::APlayerGhost()
{
	PrimaryActorTick.bCanEverTick = true;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("camera"));
	Camera->SetupAttachment(RootComponent);
}

void APlayerGhost::BeginPlay()
{
	Super::BeginPlay();
	DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	Tags.Add("PlayerGhost");
	GetWorld()->GetTimerManager().SetTimer(CheckHandle, this, &APlayerGhost::CheckIfPlayerFallOutOfWorld, 2.f, true);
	if (const TObjectPtr<APlayerController> PlayerController = Cast<APlayerController>(Controller))
	{
		if (!Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot()->GetIsTutorialFinished())
		{
			bTutorialModeEnabled = true;
			bInputLocked = true;
			(void)DelegateManager->SendTargetToTutorialDrone.ExecuteIfBound(this);
			DelegateManager->OnShowPressKeyToContinue.AddUObject(this, &APlayerGhost::SetPressKeyState);
			PlayerController->SetInputMode(FInputModeUIOnly());
			const TObjectPtr<UPlayerGhostUI> TutorialUI = GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPlayerGhostUI>(EWidget::TutorialUI);
			TutorialUI->PlayStartupAnimation();
			GetWorld()->GetTimerManager().SetTimer(EnableInputHandle, this, &APlayerGhost::EnablePlayerInput, TutorialUI->GetStartupAnimationLength(), false);
		}
	}
}

void APlayerGhost::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	SetupUserSettings();
	if (const TObjectPtr<APlayerController> PlayerController = Cast<APlayerController>(Controller))
	{
		if (const TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (!Subsystem->HasMappingContext(PlayerMappingContext)) Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}
	if (const TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(0).Action, ETriggerEvent::Triggered, this, &APlayerGhost::Move);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(4).Action, ETriggerEvent::Triggered, this, &APlayerGhost::Jump);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(5).Action, ETriggerEvent::Triggered, this, &APlayerGhost::Rotate);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(6).Action, ETriggerEvent::Started, this, &APlayerGhost::StartRunning);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(6).Action, ETriggerEvent::Completed, this, &APlayerGhost::StopRunning);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(7).Action, ETriggerEvent::Triggered, this, &APlayerGhost::PossessEnemy);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(15).Action, ETriggerEvent::Started, this, &APlayerGhost::HandleEscapeClick);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(17).Action, ETriggerEvent::Started, this, &APlayerGhost::PressKeyToContinue);
	}
}

void APlayerGhost::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(CheckHandle);
	GetWorld()->GetTimerManager().ClearTimer(EnableInputHandle);
	Super::EndPlay(EndPlayReason);
}

void APlayerGhost::Move(const FInputActionValue& InValue)
{
	if (!Controller || bInputLocked) return;
	const FVector2D MovementVector = InValue.Get<FVector2D>();
	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	AddMovementInput(GetActorRightVector(), MovementVector.X);
}

void APlayerGhost::Rotate(const FInputActionValue& InValue)
{
	if (!Controller) return;
	const FVector2D RotationVector = InValue.Get<FVector2D>();
	const FVector2D MouseSensitivity = GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetMouseSensitivity();
	AddControllerYawInput(RotationVector.X * MouseSensitivity.X);
	FRotator NewRotation = Camera->GetComponentRotation();
	NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + RotationVector.Y * MouseSensitivity.Y, -89.f, 89.f);
	Camera->SetWorldRotation(NewRotation);
}

void APlayerGhost::StartRunning() {  GetCharacterMovement()->MaxWalkSpeed = 900; }

void APlayerGhost::StopRunning() {  GetCharacterMovement()->MaxWalkSpeed = 600; }

void APlayerGhost::PossessEnemy()
{
	if (bPressKeyEnabled) return;
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	const FVector StartLocation = Camera->GetComponentLocation();
	QueryParams.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, StartLocation + Camera->GetForwardVector() * 15000.f, ECC_Visibility, QueryParams))
	{
		const TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(HitResult.GetActor());
		if (Enemy && Enemy->GetIsHijackAble() && !Enemy->GetIsMutant())
		{
			if (bTutorialModeEnabled)
			{
				(void)DelegateManager->OnShowDialogue.ExecuteIfBound(3);
				(void)DelegateManager->StopShowingHint.ExecuteIfBound();
			}
			else DelegateManager->DestroyTutorialForceField.Broadcast(1);
			(void)DelegateManager->SendTargetToTutorialDrone.ExecuteIfBound(Enemy);
			Controller->Possess(Enemy);
			Destroy();
		}
	}
}

void APlayerGhost::RestartLevel() const { UGameplayStatics::OpenLevel(GetWorld(), "Level_1"); }

void APlayerGhost::HandleEscapeClick()
{
	switch (GetWorld()->GetSubsystem<UWidgetManager>()->GetCurrentStateOfUI())
	{
		case EWidget::PauseMenuUI: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPauseMenuUI>(EWidget::PauseMenuUI)->CloseUI(); break;
		case EWidget::SettingsUI: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<USettingsUI>(EWidget::SettingsUI)->CloseUI(); break;
		default: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPauseMenuUI>(EWidget::PauseMenuUI)->OpenUI();
	}
}

void APlayerGhost::PressKeyToContinue()
{
	if (!bPressKeyEnabled) return;
	SetPressKeyState(false);
	DelegateManager->OnShowPressKeyToContinue.Broadcast(false);
}

void APlayerGhost::CheckIfPlayerFallOutOfWorld()
{
	if (GetActorLocation().Z <= -1000.f)
	{
		if (GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->IsAsyncLoadingFinished())
		{
			GetWorld()->GetTimerManager().ClearTimer(CheckHandle);
			RestartLevel();
		}
	}
}

void APlayerGhost::EnablePlayerInput() const
{
	if (!Controller) return;
	if (const TObjectPtr<APlayerController> PlayerController = Cast<APlayerController>(Controller))
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		(void)DelegateManager->OnShowHint.ExecuteIfBound(0);
		(void)DelegateManager->OnCheckDistanceToTutorialDrone.ExecuteIfBound(0);
	}
}

void APlayerGhost::SetupUserSettings() const
{
	const TObjectPtr<UEnhancedInputUserSettings> UserSettings = GetWorld()->GetFirstPlayerController()->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->GetUserSettings();
	if (!UserSettings) return;
	if (!UserSettings->IsMappingContextRegistered(PlayerMappingContext)) UserSettings->RegisterInputMappingContext(PlayerMappingContext);
}

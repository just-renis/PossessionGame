#include "PlayerGhost.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
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
	Tags.Add("PlayerGhost");
	GetWorld()->GetTimerManager().SetTimer(CheckHandle, this, &APlayerGhost::CheckIfPlayerFallOutOfWorld, 2.f, true);
	if (!Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot()->GetIsTutorialFinished())
	{
		GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPlayerGhostUI>(EWidget::TutorialUI)->PlayStartupAnimation();
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
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(5).Action, ETriggerEvent::Triggered, this, &APlayerGhost::Rotate);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(7).Action, ETriggerEvent::Triggered, this, &APlayerGhost::PossessEnemy);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(15).Action, ETriggerEvent::Started, this, &APlayerGhost::HandleEscapeClick);
	}
}

void APlayerGhost::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(CheckHandle);
	Super::EndPlay(EndPlayReason);
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

void APlayerGhost::PossessEnemy()
{
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	const FVector StartLocation = Camera->GetComponentLocation();
	QueryParams.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, StartLocation + Camera->GetForwardVector() * 15000.f, ECC_Visibility, QueryParams))
	{
		const TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(HitResult.GetActor());
		if (Enemy && Enemy->GetIsHijackAble() && !Enemy->GetIsMutant())
		{
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

void APlayerGhost::SetupUserSettings() const
{
	const TObjectPtr<UEnhancedInputUserSettings> UserSettings = GetWorld()->GetFirstPlayerController()->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->GetUserSettings();
	if (!UserSettings) return;
	if (!UserSettings->IsMappingContextRegistered(PlayerMappingContext)) UserSettings->RegisterInputMappingContext(PlayerMappingContext);
}

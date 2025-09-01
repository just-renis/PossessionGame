#include "DemoEndUI.h"
#include "PlayerHUD.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/UI/SecondaryWidgets/MainMenuButtonUI.h"
#include "PossessionGame/Utilities/Data Utilities/FLoadLevelData.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UDemoEndUI::NativeConstruct()
{
	Super::NativeConstruct();
	FadeInFinishedDelegate.BindDynamic(this, &UDemoEndUI::FadeInFinished);
	BindToAnimationFinished(FadeInAnim, FadeInFinishedDelegate);
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0.f, 1.f, 1.f, FLinearColor::Black, false, true);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = true;
	PlayerController->SetInputMode(FInputModeGameAndUI());
	PlayAnimation(FadeInAnim);
	GetWorld()->GetTimerManager().SetTimer(PauseHandle, this, &UDemoEndUI::PauseGame, 1.f);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPlayerHUD>(EWidget::PlayerHUD)->PlayFadeOutAnim(true);
}

void UDemoEndUI::PauseGame() const { UGameplayStatics::SetGamePaused(GetWorld(), true); }

void UDemoEndUI::FadeInFinished()
{
	if (ReturnToMenuButton)
	{
		ReturnToMenuButton->SetButtonVisibility(true);
		ReturnToMenuButton->Get()->OnClicked.AddDynamic(this, &UDemoEndUI::ReturnToMainMenu);
	}
	if (ReplayButton)
	{
		ReplayButton->SetButtonVisibility(true);
		ReplayButton->Get()->OnClicked.AddDynamic(this, &UDemoEndUI::Continue);
	}
	UnbindFromAnimationFinished(FadeInAnim, FadeInFinishedDelegate);
}

void UDemoEndUI::ReturnToMainMenu()
{
	GetWorld()->GetTimerManager().ClearTimer(PauseHandle);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->LoadLevel.ExecuteIfBound(FLoadLevelData(false, false, true, FName("MainMenu")));
}

void UDemoEndUI::Continue()
{
	GetWorld()->GetTimerManager().ClearTimer(PauseHandle);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->LoadLevel.ExecuteIfBound(FLoadLevelData(false, false, true, FName("Level_1")));
}

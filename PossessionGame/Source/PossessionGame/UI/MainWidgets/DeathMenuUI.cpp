#include "DeathMenuUI.h"
#include "PlayerHUD.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/UI/SecondaryWidgets/MainMenuButtonUI.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UDeathMenuUI::OpenUI()
{
	UGameplayStatics::SetGamePaused(GetWorld(), true);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::PanelFadeIn);
	const TObjectPtr<UWidgetManager> WidgetManager = GetWorld()->GetSubsystem<UWidgetManager>(); 
	if (WidgetManager->ContainsWidget(EWidget::PlayerHUD)) WidgetManager->GetWidget<UPlayerHUD>(EWidget::PlayerHUD)->PlayFadeOutAnim(true);
	PlayAnimation(FadeInAnim);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = true;
	PlayerController->SetInputMode(FInputModeGameAndUI());
	WidgetManager->SetCurrentStateOfUI(EWidget::DeathMenuUI);
}

void UDeathMenuUI::NativeConstruct()
{
	Super::NativeConstruct();
	SetupButtons();
}

void UDeathMenuUI::SetupButtons()
{
	if (RestartButton) RestartButton->Get()->OnClicked.AddDynamic(this, &UDeathMenuUI::Restart);
	if (ReturnToMenuButton) ReturnToMenuButton->Get()->OnClicked.AddDynamic(this, &UDeathMenuUI::ReturnToMainMenu);
}

void UDeathMenuUI::ReturnToMainMenu() { UGameplayStatics::OpenLevel(GetWorld(), TEXT("MainMenuLevel")); }

void UDeathMenuUI::Restart() { UGameplayStatics::OpenLevel(this, *UGameplayStatics::GetCurrentLevelName(GetWorld(), true)); }

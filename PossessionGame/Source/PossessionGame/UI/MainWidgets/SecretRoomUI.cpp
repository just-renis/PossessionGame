#include "SecretRoomUI.h"
#include "PlayerHUD.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/UI/SecondaryWidgets/CustomTextUI.h"
#include "PossessionGame/UI/SecondaryWidgets/SecretBoostCardUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void USecretRoomUI::OpenUI()
{
	ToggleUI(true);
	PlayAnimation(FadeInAnim);
	const TObjectPtr<UPossessionGameInstance> GameInstance = Cast<UPossessionGameInstance>(GetGameInstance());
	if (!GameInstance->GetSaveSlot()->GetIsExplanationShown(EExplanation::SecretUI))
	{
		(void)GetWorld()->GetSubsystem<UWidgetManager>()->GetOnShowExplanationDelegate().ExecuteIfBound(EExplanation::SecretUI, GameInstance);
	}
}

void USecretRoomUI::CloseUI()
{
	if (HpCard) HpCard->DisableButton();
	if (DamageCard) DamageCard->DisableButton();
	if (XpCard) XpCard->DisableButton();
	ToggleUI(false);
	BindToAnimationFinished(FadeOutAnim, FadeOutAnimEndDelegate);
	PlayAnimation(FadeOutAnim);
}

void USecretRoomUI::NativeConstruct()
{
	Super::NativeConstruct();
	FadeOutAnimEndDelegate.BindDynamic(this, &USecretRoomUI::FadeOutAnimFinished);
	if (HpCard) HpCard->ShowDetailsPanelContents.BindUObject(this, &USecretRoomUI::ToggleDetailsPanel);
	if (DamageCard) DamageCard->ShowDetailsPanelContents.BindUObject(this, &USecretRoomUI::ToggleDetailsPanel);
	if (XpCard) XpCard->ShowDetailsPanelContents.BindUObject(this, &USecretRoomUI::ToggleDetailsPanel);
}

void USecretRoomUI::ToggleUI(const bool bState) const
{
	UGameplayStatics::SetGamePaused(GetWorld(), bState);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(bState ? ESound::PanelFadeIn : ESound::PanelFadeOut);
	const TObjectPtr<UWidgetManager> WidgetManager = GetWorld()->GetSubsystem<UWidgetManager>(); 
	WidgetManager->GetWidget<UPlayerHUD>(EWidget::PlayerHUD)->PlayFadeOutAnim(bState);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	if (bState) PlayerController->SetInputMode(FInputModeGameAndUI());
	else PlayerController->SetInputMode(FInputModeGameOnly());
	WidgetManager->SetCurrentStateOfUI(bState ? EWidget::SecretRoomUI : EWidget::None);
}

void USecretRoomUI::ToggleDetailsPanel(const bool bState, const FText& InText) const
{
	if (!BoostDetailsPanel) return;
	BoostDetailsPanel->PlayFadeInAnim(bState);
	BoostDetailsPanel->SetText(InText);
}

void USecretRoomUI::FadeOutAnimFinished()
{
	UnbindAllFromAnimationFinished(FadeOutAnim);
	GetWorld()->GetSubsystem<UWidgetManager>()->DestroyWidget(EWidget::SecretRoomUI);
}

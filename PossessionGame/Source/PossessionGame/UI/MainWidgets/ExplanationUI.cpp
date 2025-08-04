#include "ExplanationUI.h"
#include "PlayerHUD.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/UI/SecondaryWidgets/MainMenuButtonUI.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UExplanationUI::OpenUI(const EExplanation InExplanationName)
{
	Toggle(true);
	SetCurrentExplanation(InExplanationName);
}

void UExplanationUI::CloseUI()
{
	Toggle(false);
}

void UExplanationUI::NativeConstruct()
{
	Super::NativeConstruct();
	NextPageButton->Get()->OnClicked.AddDynamic(this, &UExplanationUI::NextPage);
	BackButton->Get()->OnClicked.AddDynamic(this, &UExplanationUI::BackPage);
	FinishButton->Get()->OnClicked.AddDynamic(this, &UExplanationUI::Finish);
}

void UExplanationUI::Toggle(const bool bState)
{
	UGameplayStatics::SetGamePaused(GetWorld(), bState);
	SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	BackButton->SetButtonVisibility(bState);
	FinishButton->SetButtonVisibility(bState);
	NextPageButton->SetButtonVisibility(bState);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(bState ? ESound::PanelFadeIn : ESound::PanelFadeOut);
	const TObjectPtr<UWidgetManager> WidgetManager = GetWorld()->GetSubsystem<UWidgetManager>(); 
	if (WidgetManager->ContainsWidget(EWidget::PlayerHUD)) WidgetManager->GetWidget<UPlayerHUD>(EWidget::PlayerHUD)->PlayFadeOutAnim(bState);
	if (bState) PlayAnimation(FadeInAnim);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	if (bState) PlayerController->SetInputMode(FInputModeGameAndUI());
	else PlayerController->SetInputMode(FInputModeGameOnly());
	WidgetManager->SetCurrentStateOfUI(bState ? EWidget::ExplanationUI : EWidget::None);
}

void UExplanationUI::ShowExplanation() const
{
	if (CurrentExplanationData.GetPageAmount() > 0)
	{
		PageText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "PageDisplay", "{0} / {1}"),
			FText::AsNumber(CurrentPage + 1), FText::AsNumber(CurrentExplanationData.GetPageAmount())));
		ExplanationText->SetText(CurrentExplanationData.GetExplanationText(CurrentPage));
	}
}

void UExplanationUI::SetCurrentExplanation(const EExplanation InExplanationName)
{
	if (const FExplanationData* ExplanationData = Explanations.Find(InExplanationName)) CurrentExplanationData = *ExplanationData;
	else CurrentExplanationData = FExplanationData();
	ShowExplanation();
}

void UExplanationUI::NextPage()
{
	if (CurrentPage + 1 >= CurrentExplanationData.GetPageAmount()) return;
	CurrentPage++;
	ShowExplanation();
}

void UExplanationUI::BackPage()
{
	if (CurrentPage - 1 < 0) return;
	CurrentPage--;
	ShowExplanation();
}

void UExplanationUI::Finish()
{
	CloseUI();
}

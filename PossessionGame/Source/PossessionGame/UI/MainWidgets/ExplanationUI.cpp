#include "ExplanationUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/UI/SecondaryWidgets/ExplanationHighlight.h"
#include "PossessionGame/UI/SecondaryWidgets/MainMenuButtonUI.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/TutorialManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UExplanationUI::OpenUI(const EExplanation InExplanationName)
{
	Toggle(true);
	SetCurrentExplanation(InExplanationName);
}

void UExplanationUI::NativeConstruct()
{
	Super::NativeConstruct();
	NextPageButton->Get()->OnClicked.AddDynamic(this, &UExplanationUI::NextPage);
	BackButton->Get()->OnClicked.AddDynamic(this, &UExplanationUI::BackPage);
	FinishButton->Get()->OnClicked.AddDynamic(this, &UExplanationUI::CloseUI);
}

void UExplanationUI::Toggle(const bool bState)
{
	BackButton->SetButtonVisibility(bState);
	FinishButton->SetButtonVisibility(bState);
	NextPageButton->SetButtonVisibility(bState);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(bState ? ESound::PanelFadeIn : ESound::PanelFadeOut);
	const TObjectPtr<UWidgetManager> WidgetManager = GetWorld()->GetSubsystem<UWidgetManager>();
	WidgetManager->SetExplanationState(bState);
	if (bState) PreviousWidgetState = WidgetManager->GetCurrentStateOfUI();
	WidgetManager->SetCurrentStateOfUI(bState ? EWidget::ExplanationUI : PreviousWidgetState);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	switch (WidgetManager->GetCurrentStateOfUI())
	{
		case EWidget::UpgradeSelectionUI: case EWidget::VariationsUI: case EWidget::SecretRoomUI: case EWidget::AbilitiesMenuUI:
			if (!bState) return;
			if (Highlight) Highlight->PlayFadeAnim(true);
			SetVisibility(ESlateVisibility::Visible);
			break;
		default:
			UGameplayStatics::SetGamePaused(GetWorld(), bState);
			PlayerController->bShowMouseCursor = bState;
			if (bState)
			{
				PlayerController->SetInputMode(FInputModeGameAndUI());
				SetVisibility(ESlateVisibility::Visible);
				if (Highlight) Highlight->PlayFadeAnim(true);
			}
			else PlayerController->SetInputMode(FInputModeGameOnly());
	}
}

void UExplanationUI::ShowExplanation()
{
	if (CurrentExplanationData.GetPageAmount() > 0)
	{
		PageText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "PageDisplay", "Page {0} / {1}"), FText::AsNumber(CurrentPage + 1), FText::AsNumber(CurrentExplanationData.GetPageAmount())));
		ExplanationText->SetText(CurrentExplanationData.GetExplanationText(CurrentPage));
		ChangeButtonsStates(CurrentPage);
		if (const TObjectPtr<UTexture2D> HighLightTexture = CurrentExplanationData.GetHighlightTexture(CurrentPage)) Highlight->SetTexture(HighLightTexture);
	}
}

void UExplanationUI::SetCurrentExplanation(const EExplanation InExplanationName)
{
	if (const FExplanationData* ExplanationData = Explanations.Find(InExplanationName)) CurrentExplanationData = *ExplanationData;
	else CurrentExplanationData = FExplanationData();
	PreviousPage = -1;
	CurrentPage = 0;
	ChangeButtonsStates(CurrentPage);
	if (CurrentExplanationData.GetPageAmount() > 1) FinishButton->SetVisibility(ESlateVisibility::Hidden);
	ShowExplanation();
	PlayExplanationAnim(InExplanationName);
}

void UExplanationUI::PlayExplanationAnim(const EExplanation InExplanationName)
{
	switch (InExplanationName)
	{
		case EExplanation::FpsUI: case EExplanation::VariationDetailsUI: PlayAnimation(FpsFadeInAnim); break;
		case EExplanation::AbilitiesUI: case EExplanation::SecretUI: case EExplanation::UpgradeUI: case EExplanation::UpgradeChoiceUI: case EExplanation::VariationMainUI:
		PlayAnimation(AbilitiesUpgradeVariationFadeInAnim);
	}
}

void UExplanationUI::CloseExplanationLogic()
{
	if (Highlight) Highlight->PlayFadeAnim(false);
	FinishedFadeOutDelegate.BindDynamic(this, &UExplanationUI::FinishedFadeOutAnim);
	switch (CurrentExplanationData.GetExplanationName())
	{
		case EExplanation::FpsUI: case EExplanation::VariationDetailsUI:
			PlayAnimation(FpsFadeOutAnim);
			GetWorld()->GetSubsystem<UTutorialManager>()->OnShowHint.Broadcast();
			BindToAnimationFinished(FpsFadeOutAnim, FinishedFadeOutDelegate);
			break;
		case EExplanation::AbilitiesUI: case EExplanation::SecretUI: case EExplanation::UpgradeUI: case EExplanation::UpgradeChoiceUI: case EExplanation::VariationMainUI:
			PlayAnimation(AbilitiesUpgradeVariationFadeOutAnim);
			BindToAnimationFinished(AbilitiesUpgradeVariationFadeOutAnim, FinishedFadeOutDelegate);
	}
}

void UExplanationUI::ChangeButtonsStates(const int32 InCurrentPage) const
{
	BackButton->SetVisibility(InCurrentPage ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
	NextPageButton->SetVisibility(InCurrentPage >= CurrentExplanationData.GetPageAmount() - 1 ? ESlateVisibility::Hidden : ESlateVisibility::SelfHitTestInvisible);
}

void UExplanationUI::NextPage()
{
	if (CurrentPage + 1 > CurrentExplanationData.GetPageAmount()) return;
	PreviousPage = CurrentPage;
	PlayAnimation(BackNextTextAnim);
	if (++CurrentPage + 1 == CurrentExplanationData.GetPageAmount()) FinishButton->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	ChangeButtonsStates(CurrentPage);
	ShowExplanation();
}

void UExplanationUI::BackPage()
{
	if (CurrentPage - 1 < 0) return;
	PreviousPage = CurrentPage;
	PlayAnimation(BackNextTextAnim);
	CurrentPage--;
	ChangeButtonsStates(CurrentPage);
	ShowExplanation();
}

void UExplanationUI::CloseUI()
{
	Toggle(false);
	CloseExplanationLogic();
}

void UExplanationUI::FinishedFadeOutAnim()
{
	ExplanationBox->SetRenderTranslation(FVector2D(0.f, 0.f));
	ExplanationOverlay->SetRenderTranslation(FVector2D(0.f, 0.f));
	SetVisibility(ESlateVisibility::Collapsed);
	UnbindAllFromAnimationFinished(FpsFadeOutAnim);
	UnbindAllFromAnimationFinished(AbilitiesUpgradeVariationFadeOutAnim);
}

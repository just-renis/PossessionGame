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
	UGameplayStatics::SetGamePaused(GetWorld(), bState);
	BackButton->SetButtonVisibility(bState);
	FinishButton->SetButtonVisibility(bState);
	NextPageButton->SetButtonVisibility(bState);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(bState ? ESound::PanelFadeIn : ESound::PanelFadeOut);
	const TObjectPtr<UWidgetManager> WidgetManager = GetWorld()->GetSubsystem<UWidgetManager>(); 
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	if (bState)
	{
		PlayerController->SetInputMode(FInputModeGameAndUI());
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else PlayerController->SetInputMode(FInputModeGameOnly());
	WidgetManager->SetCurrentStateOfUI(bState ? EWidget::ExplanationUI : EWidget::None);
}

void UExplanationUI::ShowExplanation()
{
	if (CurrentExplanationData.GetPageAmount() > 0)
	{
		PageText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "PageDisplay", "Page {0} / {1}"), FText::AsNumber(CurrentPage + 1), FText::AsNumber(CurrentExplanationData.GetPageAmount())));
		ExplanationText->SetText(CurrentExplanationData.GetExplanationText(CurrentPage));
		if (const TObjectPtr<UTexture2D> HighLightTexture = CurrentExplanationData.GetHighlightTexture(CurrentPage)) Highlight->SetTexture(HighLightTexture);
	}
}

void UExplanationUI::SetCurrentExplanation(const EExplanation InExplanationName)
{
	if (const FExplanationData* ExplanationData = Explanations.Find(InExplanationName)) CurrentExplanationData = *ExplanationData;
	else CurrentExplanationData = FExplanationData();
	PreviousPage = -1;
	CurrentPage = 0;
	ShowExplanation();
	PlayExplanationAnim(InExplanationName);
}

void UExplanationUI::PlayExplanationAnim(const EExplanation InExplanationName)
{
	switch (InExplanationName)
	{
		case EExplanation::FpsUI: PlayAnimation(FpsFadeInAnim); break;
		case EExplanation::AbilitiesUI: case EExplanation::SecretUI: case EExplanation::UpgradeUI: PlayAnimation(AbilitiesUpgradeVariationFadeInAnim); break;
		case EExplanation::VariationUI: PlayAnimation(VariationSlideRightAnim);
	}
}

void UExplanationUI::CloseExplanationLogic()
{
	FinishedFadeOutDelegate.BindDynamic(this, &UExplanationUI::FinishedFadeOutAnim);
	switch (CurrentExplanationData.GetExplanationName())
	{
		case EExplanation::FpsUI:
			PlayAnimation(FpsFadeOutAnim);
			(void)GetWorld()->GetSubsystem<UTutorialManager>()->OnShowHint.ExecuteIfBound();
			BindToAnimationFinished(FpsFadeOutAnim, FinishedFadeOutDelegate);
			break;
		case EExplanation::AbilitiesUI: case EExplanation::SecretUI: case EExplanation::UpgradeUI:
			PlayAnimation(AbilitiesUpgradeVariationFadeOutAnim);
			BindToAnimationFinished(AbilitiesUpgradeVariationFadeOutAnim, FinishedFadeOutDelegate);
			break;
		case EExplanation::VariationUI:
			PlayAnimation(VariationRightFadeOutAnim);
			BindToAnimationFinished(VariationRightFadeOutAnim, FinishedFadeOutDelegate);
	}
}

void UExplanationUI::NextPage()
{
	if (CurrentPage + 1 >= CurrentExplanationData.GetPageAmount()) return;
	PreviousPage = CurrentPage;
	PlayAnimation(BackNextTextAnim);
	CurrentPage++;
	ShowExplanation();
}

void UExplanationUI::BackPage()
{
	if (CurrentPage - 1 < 0) return;
	PreviousPage = CurrentPage;
	PlayAnimation(BackNextTextAnim);
	CurrentPage--;
	ShowExplanation();
}

void UExplanationUI::CloseUI()
{
	Toggle(false);
	CloseExplanationLogic();
}

void UExplanationUI::FinishedFadeOutAnim()
{
	SetVisibility(ESlateVisibility::Collapsed);
	UnbindAllFromAnimationFinished(FpsFadeOutAnim);
	UnbindAllFromAnimationFinished(AbilitiesUpgradeVariationFadeOutAnim);
	UnbindAllFromAnimationFinished(VariationRightFadeOutAnim);
}

#include "ModeSelectUI.h"
#include "MainMenuButtonUI.h"
#include "Components/Button.h"
#include "PossessionGame/UI/MainWidgets/SaveSlotMenuUI.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

TObjectPtr<UButton> UModeSelectUI::GetBackButton() const { return BackButton->Get(); }

void UModeSelectUI::PlayFadeInAnim(const bool bState)
{
	PlayAnimation(FadeInAnim, GetAnimationCurrentTime(FadeInAnim), 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(bState ? ESound::PanelFadeIn : ESound::PanelFadeOut);
	if (bState) SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	else BindToAnimationFinished(FadeInAnim, FadeOutDelegate);
	if (StoryModeButton) StoryModeButton->SetButtonVisibility(bState);
	if (ArenaModeButton) ArenaModeButton->SetButtonVisibility(bState);
	if (BackButton) BackButton->SetButtonVisibility(bState);
}

void UModeSelectUI::NativeConstruct()
{
	Super::NativeConstruct();
	StoryModeButton->Get()->OnClicked.AddDynamic(this, &UModeSelectUI::OpenStoryMode);
	ArenaModeButton->Get()->OnClicked.AddDynamic(this, &UModeSelectUI::OpenArenaMode);
	FadeOutDelegate.BindDynamic(this, &UModeSelectUI::FadeOutAnimFinished);
}

void UModeSelectUI::OpenStoryMode()
{
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<USaveSlotMenuUI>(EWidget::SaveSlotMenuUI)->OpenUI(true);
	PlayFadeInAnim(false);
}

void UModeSelectUI::OpenArenaMode()
{
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<USaveSlotMenuUI>(EWidget::SaveSlotMenuUI)->OpenUI(false);
	PlayFadeInAnim(false);
}

void UModeSelectUI::FadeOutAnimFinished()
{
	UnbindAllFromAnimationFinished(FadeInAnim);
	SetVisibility(ESlateVisibility::Collapsed);
}

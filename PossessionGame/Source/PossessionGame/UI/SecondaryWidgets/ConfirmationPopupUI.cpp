#include "ConfirmationPopupUI.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UConfirmationPopupUI::PlayPopupFadeInAnim(const bool bState)
{
	SetIsEnabled(bState);
	PlayAnimation(PopupFadeIn, GetAnimationCurrentTime(PopupFadeIn), 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(bState ? ESound::PanelFadeIn :ESound::PanelFadeOut);
	if (bState) SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	else BindToAnimationFinished(PopupFadeIn, FadeOutDelegate);
}

void UConfirmationPopupUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (ConfirmationTitle) ConfirmationTitle->SetText(ConfirmationTitleText);
	if (AcceptButtonText) AcceptButtonText->SetText(AcceptText);
	if (DeclineButtonText) DeclineButtonText->SetText(DeclineText);
}

void UConfirmationPopupUI::NativeConstruct()
{
	Super::NativeConstruct();
	FadeOutDelegate.BindDynamic(this, &UConfirmationPopupUI::FadeOutAnimFinished);
}

void UConfirmationPopupUI::FadeOutAnimFinished()
{
	UnbindAllFromAnimationFinished(PopupFadeIn);
	SetVisibility(ESlateVisibility::Collapsed);
}

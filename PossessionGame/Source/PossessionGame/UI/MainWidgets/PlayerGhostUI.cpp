#include "PlayerGhostUI.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/InvalidationBox.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

bool UPlayerGhostUI::IsPressKeyShown() const { return PressKeyToContinueInv->GetVisibility() == ESlateVisibility::SelfHitTestInvisible; }

float UPlayerGhostUI::GetStartupAnimationLength() const { return StartupAnim ? StartupAnim->GetEndTime() : 0.f; }

void UPlayerGhostUI::PlayStartupAnimation() { if (StartupAnim) PlayAnimation(StartupAnim); }

void UPlayerGhostUI::NativeConstruct()
{
	Super::NativeConstruct();
	DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	DelegateManager->OnShowHint.BindUObject(this, &UPlayerGhostUI::ShowHint);
	DelegateManager->StopShowingHint.BindUObject(this, &UPlayerGhostUI::StopShowingHint);
	DelegateManager->OnShowPressKeyToContinue.AddUObject(this, &UPlayerGhostUI::ShowPressKey);
}

void UPlayerGhostUI::ShowHint(const int32 InIndex)
{
	if (HintTexts.IsValidIndex(InIndex))
	{
		if (HintAppearAnim) PlayAnimation(HintAppearAnim);
		SetHintText(HintTexts[InIndex]);
	}
	if (InIndex == 0 && PopupInv && HintImage)
	{
		PopupInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		if (PopupAppearAnim) PlayAnimation(PopupAppearAnim);
	}
}

void UPlayerGhostUI::SetHintText(const FString& InText) const
{
	if (HintInv) HintInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (HintText) HintText->SetText(FText::FromString(InText));
}

void UPlayerGhostUI::StopShowingHint() const
{
	if (HintInv) HintInv->SetVisibility(ESlateVisibility::Collapsed);
	if (PopupInv) PopupInv->SetVisibility(ESlateVisibility::Collapsed);
	if (HintImage) HintImage->SetVisibility(ESlateVisibility::Collapsed);
}

void UPlayerGhostUI::ShowPressKey(const bool bState)
{
	if (PressKeyToContinueInv) PressKeyToContinueInv->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	if (bState)
	{
		if (PressKeyAppearAnim) PlayAnimation(PressKeyAppearAnim);
		return;
	}
	switch (CurrentTutorialStep++)
	{
		case 0: (void)DelegateManager->OnShowDialogue.ExecuteIfBound(1); break;
		case 1: (void)DelegateManager->OnShowDialogue.ExecuteIfBound(2); (void)DelegateManager->OnShowHint.ExecuteIfBound(1); break;
		case 2: (void)DelegateManager->StartTutorialDroneMoveToLocation.ExecuteIfBound(0, false); break;
		case 3: (void)DelegateManager->OnShowDialogue.ExecuteIfBound(6); break;
		case 4:
			DelegateManager->OnTutorialEnd.Broadcast();
			GetWorld()->GetSubsystem<UWidgetManager>()->DestroyWidget(EWidget::TutorialUI);
			DelegateManager->OnShowPressKeyToContinue.RemoveAll(this);
			break;
		default: ;
	}
}

#include "PlayerGhostUI.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/InvalidationBox.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

bool UPlayerGhostUI::IsPressAnyKeyShown() const { return PressAnyKeyToContinueInv->GetVisibility() == ESlateVisibility::SelfHitTestInvisible; }

float UPlayerGhostUI::GetStartupAnimationLength() const { return StartupAnimation ? StartupAnimation->GetEndTime() : 0.f; }

void UPlayerGhostUI::PlayStartupAnimation() { if (StartupAnimation) PlayAnimation(StartupAnimation); }

void UPlayerGhostUI::NativeConstruct()
{
	Super::NativeConstruct();
	DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	DelegateManager->OnShowHint.BindUObject(this, &UPlayerGhostUI::ShowHint);
	DelegateManager->StopShowingHint.BindUObject(this, &UPlayerGhostUI::StopShowingHint);
	DelegateManager->OnShowPressAnyKey.BindUObject(this, &UPlayerGhostUI::ShowPressAnyKey);
	DelegateManager->OnTutorialStepChange.BindUObject(this, &UPlayerGhostUI::SetCurrentTutorialStep);
}

FReply UPlayerGhostUI::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (PressAnyKeyToContinueInv && IsPressAnyKeyShown())
	{
		ShowPressAnyKey(false);
		switch (CurrentTutorialStep)
		{
			case 0: (void)DelegateManager->OnShowDialogue.ExecuteIfBound(1); break;
			case 1: (void)DelegateManager->StartTutorialDroneMoveToLocation.ExecuteIfBound(0); break;
			case 2: (void)DelegateManager->OnShowDialogue.ExecuteIfBound(6); break;
			case 3:
				(void)DelegateManager->OpenTutorialGate.ExecuteIfBound();
				DelegateManager->OnTutorialEnd.Broadcast();
				break;
			default: ;
		}
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

FReply UPlayerGhostUI::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return bIsShowPressAnyKeyEnabled ? FReply::Handled() : Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UPlayerGhostUI::ShowHint(const int32 InIndex)
{
	if (HintTexts.IsValidIndex(InIndex)) SetHintText(HintTexts[InIndex]);
	if (InIndex == 0 && PopupInv) PopupInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
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
}

void UPlayerGhostUI::ShowPressAnyKey(const bool bState)
{
	if (PressAnyKeyToContinueInv) PressAnyKeyToContinueInv->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	if (const TObjectPtr<APlayerController> Controller = GetWorld()->GetFirstPlayerController())
	{
		bIsShowPressAnyKeyEnabled = bState;
		if (bState)
		{
			Controller->SetInputMode(FInputModeUIOnly());
			SetKeyboardFocus();
		}
		else Controller->SetInputMode(FInputModeGameOnly());
	}
}

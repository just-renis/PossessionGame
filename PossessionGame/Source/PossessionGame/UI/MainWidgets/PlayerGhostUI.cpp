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
	DelegateManager->OnShowDialogue.AddUObject(this, &UPlayerGhostUI::StopShowingHint);
	DelegateManager->OnShowPressAnyKey.BindUObject(this, &UPlayerGhostUI::ShowPressAnyKey);
}

FReply UPlayerGhostUI::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (PressAnyKeyToContinueInv && IsPressAnyKeyShown())
	{
		ShowPressAnyKey(false);
		switch (CurrentTutorialStep)
		{
			case 0: DelegateManager->OnShowDialogue.Broadcast(1); break;
			case 1: (void)DelegateManager->StartTutorialDroneMoveToLocation.ExecuteIfBound(0); break;
			default: ;
		}
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UPlayerGhostUI::ShowHint(const int32 InIndex)
{
	if (HintTexts.IsValidIndex(InIndex)) SetHintText(HintTexts[InIndex]);
	if (InIndex == 0 && PopupInv) PopupInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	CurrentTutorialStep = InIndex;
}

void UPlayerGhostUI::SetHintText(const FString& InText) const
{
	if (HintInv) HintInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (HintText) HintText->SetText(FText::FromString(InText));
}

void UPlayerGhostUI::StopShowingHint(const int32 InIndex) const
{
	if (InIndex != 0 && InIndex != 3) return;
	if (HintInv) HintInv->SetVisibility(ESlateVisibility::Collapsed);
	if (InIndex == 0 && PopupInv) PopupInv->SetVisibility(ESlateVisibility::Collapsed);
}

void UPlayerGhostUI::ShowPressAnyKey(const bool bState)
{
	if (PressAnyKeyToContinueInv) PressAnyKeyToContinueInv->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	SetIsFocusable(bState);
	if (bState) UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(GetWorld()->GetFirstPlayerController(), this);
	else UWidgetBlueprintLibrary::SetInputMode_GameOnly(GetWorld()->GetFirstPlayerController());
}

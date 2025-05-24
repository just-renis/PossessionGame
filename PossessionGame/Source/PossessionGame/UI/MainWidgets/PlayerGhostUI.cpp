#include "PlayerGhostUI.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/InvalidationBox.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/GlobalSettingsManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

bool UPlayerGhostUI::IsPressKeyShown() const { return PressKeyToContinueInv->GetVisibility() == ESlateVisibility::SelfHitTestInvisible; }

float UPlayerGhostUI::GetStartupAnimationLength() const { return StartupAnim ? StartupAnim->GetEndTime() : 0.f; }

void UPlayerGhostUI::PlayStartupAnimation() { if (StartupAnim) PlayAnimation(StartupAnim); }

FText UPlayerGhostUI::GetKeyBindText(const int32 InIndex) const
{
	const FName KeyBindName = InIndex == 1 ? "Hijack" : InIndex == 2 ? "M1" : "Shift";
	return GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetKeyBindText(KeyBindName);
}

void UPlayerGhostUI::NativeConstruct()
{
	Super::NativeConstruct();
	DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	DelegateManager->OnShowHint.BindUObject(this, &UPlayerGhostUI::ShowHint);
	DelegateManager->StopShowingHint.BindUObject(this, &UPlayerGhostUI::StopShowingHint);
	DelegateManager->OnShowPressKeyToContinue.AddUObject(this, &UPlayerGhostUI::ShowPressKey);
	DelegateManager->OnKeyBindChange.AddUObject(this, &UPlayerGhostUI::UpdateKeyBindText);
}

void UPlayerGhostUI::ShowHint(const int32 InIndex)
{
	if (HintTexts.IsValidIndex(InIndex))
	{
		if (HintAppearAnim) PlayAnimation(HintAppearAnim);
		CurrentHintIndex = InIndex;
		SetHintText(InIndex);
	}
	if (InIndex == 0 && PopupInv && HintImage)
	{
		PopupInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		if (PopupAppearAnim) PlayAnimation(PopupAppearAnim);
	}
}

void UPlayerGhostUI::SetHintText(const int32 InIndex) const
{
	if (!HintInv || !HintText) return;
	HintInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	const FText KeyBindText = GetKeyBindText(InIndex);
	switch (InIndex)
	{
		case 0: HintText->SetText(FText::FromString("Move")); break;
		case 1: HintText->SetText(FText::Format(FText::FromString("[{0}] to Possess Robots"), KeyBindText)); break;
		case 2: HintText->SetText(FText::Format(FText::FromString("[{0}] to Shoot"), KeyBindText)); break;
		case 3: HintText->SetText(FText::Format(FText::FromString("[{0}] to Sprint"), KeyBindText));
		default: ;
	}
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
	if (PressKeyToContinueText) PressKeyToContinueText->SetText(FText::Format(FText::FromString("Press [{0}] Key To Continue"), GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetKeyBindText("Interact")));
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
			DelegateManager->OnKeyBindChange.RemoveAll(this);
			DelegateManager->OnShowPressKeyToContinue.RemoveAll(this);
			GetWorld()->GetSubsystem<UWidgetManager>()->DestroyWidget(EWidget::TutorialUI);
			break;
		default: ;
	}
}

void UPlayerGhostUI::UpdateKeyBindText(const FName& InKeyBindName, const FText& InKeyBindText) const
{
	if (PressKeyToContinueText && InKeyBindName.IsEqual("Interact")) PressKeyToContinueText->SetText(FText::Format(FText::FromString("Press [{0}] Key To Continue"), InKeyBindText));
	if (!HintText) return;
	if (CurrentHintIndex == 1 && InKeyBindName.IsEqual("Hijack")) HintText->SetText(FText::Format(FText::FromString("[{0}] to Possess Robots"), InKeyBindText));
	else if (CurrentHintIndex == 2 && InKeyBindName.IsEqual("M1")) HintText->SetText(FText::Format(FText::FromString("[{0}] to Shoot"), InKeyBindText));
	else if (CurrentHintIndex == 3 && InKeyBindName.IsEqual("Shift")) HintText->SetText(FText::Format(FText::FromString("[{0}] to Sprint"), InKeyBindText));
}

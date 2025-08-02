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

float UPlayerGhostUI::GetStartupAnimationLength() const { return StartupAnim->GetEndTime(); }

void UPlayerGhostUI::PlayStartupAnimation() { PlayAnimation(StartupAnim); }

FText UPlayerGhostUI::GetKeyBindText(const int32 InIndex) const
{
	switch (InIndex)
	{
		case 0: return GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetKeyBindText("Hijack");
		case 2: return GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetKeyBindText("Shoot");
		case 3: return GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetKeyBindText("Dash");
		case 4: return GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetKeyBindText("Jump");
		case 5: return GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetKeyBindText("Stomp");
		default: ;
	}
	return FText();
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

void UPlayerGhostUI::ShowHint()
{
	if (!HintTexts.IsValidIndex(CurrentHintIndex)) return;
	PlayAnimation(HintAppearAnim);
	SetHintText(CurrentHintIndex++);
	if (CurrentHintIndex != 2 || !PopupInv || !HintImage) return;
	HintImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	PopupInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	PlayAnimation(PopupAppearAnim);
}

void UPlayerGhostUI::SetHintText(const int32 InIndex) const
{
	if (!HintInv || !HintText) return;
	HintInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	const FText KeyBindText = GetKeyBindText(InIndex);
	switch (InIndex)
	{
		case 0: HintText->SetText(FText::Format(FText::FromString("[{0}] to Hijack Robots"), KeyBindText)); break;
		case 1: HintText->SetText(FText::FromString("Move")); break;
		case 2: HintText->SetText(FText::Format(FText::FromString("[{0}] to Shoot"), KeyBindText)); break;
		case 3: HintText->SetText(FText::Format(FText::FromString("[{0}] to Dash"), KeyBindText)); break;
		case 4: HintText->SetText(FText::Format(FText::FromString("Double Press [{0}] to Double Jump"), KeyBindText)); break;
		case 5: HintText->SetText(FText::Format(FText::FromString("Double Jump then Press [{0}] to Stomp"), KeyBindText));
		default: ;
	}
}

void UPlayerGhostUI::StopShowingHint()
{
	if (HintInv) HintInv->SetVisibility(ESlateVisibility::Collapsed);
	if (PopupInv) PopupInv->SetVisibility(ESlateVisibility::Collapsed);
	if (HintImage) HintImage->SetVisibility(ESlateVisibility::Collapsed);
	if (CurrentHintIndex >= 5) DestroyThis();
}

void UPlayerGhostUI::ShowPressKey(const bool bState)
{
	if (PressKeyToContinueInv) PressKeyToContinueInv->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	if (PressKeyToContinueText) PressKeyToContinueText->SetText(FText::Format(FText::FromString("Press [{0}] Key To Continue"), GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetKeyBindText("Interact")));
	if (bState) PlayAnimation(PressKeyAppearAnim);
}

void UPlayerGhostUI::UpdateKeyBindText(const FName& InKeyBindName, const FText& InKeyBindText) const
{
	if (PressKeyToContinueText && InKeyBindName.IsEqual("Interact")) PressKeyToContinueText->SetText(FText::Format(FText::FromString("Press [{0}] Key To Continue"), InKeyBindText));
	if (!HintText) return;
	switch (CurrentHintIndex)
	{
		case 1: if (InKeyBindName.IsEqual("Hijack")) HintText->SetText(FText::Format(FText::FromString("[{0}] to Hijack Robots"), InKeyBindText)); break;
		case 2: if (InKeyBindName.IsEqual("Shoot")) HintText->SetText(FText::Format(FText::FromString("[{0}] to Shoot"), InKeyBindText)); break;
		case 3: if (InKeyBindName.IsEqual("Dash")) HintText->SetText(FText::Format(FText::FromString("[{0}] to Dash"), InKeyBindText)); break;
		case 4: if (InKeyBindName.IsEqual("Jump")) HintText->SetText(FText::Format(FText::FromString("Double Press [{0}] to Double Jump"), InKeyBindText)); break;
		case 5: if (InKeyBindName.IsEqual("Stomp")) HintText->SetText(FText::Format(FText::FromString("Double Jump then Press [{0}] to Stomp"), InKeyBindText));
		default: ;
	}
}

void UPlayerGhostUI::DestroyThis()
{
	DelegateManager->OnKeyBindChange.RemoveAll(this);
	GetWorld()->GetSubsystem<UWidgetManager>()->DestroyWidget(EWidget::TutorialUI);
}

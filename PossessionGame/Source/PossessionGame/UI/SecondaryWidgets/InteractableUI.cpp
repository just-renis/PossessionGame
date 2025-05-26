#include "InteractableUI.h"
#include "Animation/WidgetAnimation.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/GlobalSettingsManager.h"

float UInteractableUI::GetFadeInAnimLength() const { return FadeInAnim ? FadeInAnim->GetEndTime() : 0.1f; }

void UInteractableUI::PlayFadeInAnim(const bool bState)
{
	if (!FadeInAnim) return;
	PlayAnimation(FadeInAnim, 0, 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
}

void UInteractableUI::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateInteractableText("Interact", GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetKeyBindText("Interact"));
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnKeyBindChange.AddUObject(this, &UInteractableUI::UpdateInteractableText);
}

void UInteractableUI::UpdateInteractableText(const FName& InKeyBindName, const FText& InKeyBindText)
{
	if (!InteractableText || !InKeyBindName.IsEqual("Interact")) return;
	InteractableText->SetText(FText::Format(FText::FromString("Press [{0}] to interact"), InKeyBindText));
}

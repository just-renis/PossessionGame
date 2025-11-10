#include "InteractableUI.h"
#include "Animation/WidgetAnimation.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/GlobalSettingsManager.h"

float UInteractableUI::GetFadeInAnimLength() const { return FadeInAnim->GetEndTime(); }

void UInteractableUI::PlayFadeInAnim(const bool bState)
{
	PlayAnimation(FadeInAnim, 0, 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
}

void UInteractableUI::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateInteractableText(FName("Interact"), GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetKeyBindText("Interact"));
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnKeyBindChange.AddUObject(this, &UInteractableUI::UpdateInteractableText);
}

void UInteractableUI::UpdateInteractableText(const FName& InKeyBindName, const FText& InKeyBindText)
{
	if (!InteractableText || !InKeyBindName.IsEqual("Interact")) return;
	InteractableText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "PressToInteract", "Press [{0}] to interact"), InKeyBindText));
}

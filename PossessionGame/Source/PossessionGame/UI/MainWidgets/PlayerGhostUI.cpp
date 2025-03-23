#include "PlayerGhostUI.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

void UPlayerGhostUI::NativeConstruct()
{
	Super::NativeConstruct();
	DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	DelegateManager->OnFinishedDialogue.BindUObject(this, &UPlayerGhostUI::ShowHint);
}

void UPlayerGhostUI::ShowHint(const int32 InIndex)
{
	if (HintTexts.IsValidIndex(InIndex)) SetHintText(HintTexts[InIndex]);
}

void UPlayerGhostUI::SetHintText(const FString& InText) const
{
	if (HintText) HintText->SetText(FText::FromString(InText));
}

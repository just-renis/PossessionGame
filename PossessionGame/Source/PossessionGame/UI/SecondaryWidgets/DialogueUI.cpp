#include "DialogueUI.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

void UDialogueUI::StartDialogue(const int32 InIndex, const float InSpeed)
{
	if (!DialogueTexts.IsValidIndex(InIndex)) return;
	DialogueTextIndex = InIndex;
	TargetText = DialogueTexts[DialogueTextIndex];
	CurrentText = "";
	CurrentIndex = 0;
	TypeSpeed = InSpeed;
	UKismetSystemLibrary::K2_SetTimer(this, "TypeNextLetter", TypeSpeed, true);
}

void UDialogueUI::NativeConstruct()
{
	Super::NativeConstruct();
	DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
    if (DialogueText) DialogueText->SetText(FText::FromString(""));
}

void UDialogueUI::TypeNextLetter()
{
	if (!DialogueText) return;
	if (CurrentIndex < TargetText.Len())
	{
		CurrentText.AppendChar(TargetText[CurrentIndex++]);
		DialogueText->SetText(FText::FromString(CurrentText));
	}
	else
	{
		UKismetSystemLibrary::K2_ClearTimer(this, "TypeNextLetter");
		if (DialogueTextIndex == 0) (void)DelegateManager->OnShowPressAnyKey.ExecuteIfBound(true);
	}
}

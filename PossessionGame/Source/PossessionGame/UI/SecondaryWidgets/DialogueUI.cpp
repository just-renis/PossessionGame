#include "DialogueUI.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UDialogueUI::StartDialogue(const int32 InIndex, const float InSpeed)
{
	if (!DialogueTexts.IsValidIndex(InIndex)) return;
	if (GetVisibility() == ESlateVisibility::Collapsed) SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	AssetsManager->PlaySound2D(ESound::DialogueStart);
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
	AssetsManager = GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>();
    if (DialogueText) DialogueText->SetText(FText::FromString(""));
}

void UDialogueUI::FinishedTypingDialogue() { UKismetSystemLibrary::K2_ClearTimer(this, "TypeNextLetter"); }

void UDialogueUI::TypeNextLetter()
{
	if (!DialogueText) return;
	if (CurrentIndex < TargetText.Len())
	{
		CurrentText.AppendChar(TargetText[CurrentIndex++]);
		DialogueText->SetText(FText::FromString(CurrentText));
	}
	else FinishedTypingDialogue();
}

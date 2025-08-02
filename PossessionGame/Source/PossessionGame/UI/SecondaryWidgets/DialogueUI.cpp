#include "DialogueUI.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UDialogueUI::ClearDialogueText() const { if (DialogueText) DialogueText->SetText(FText::FromString("")); }

void UDialogueUI::StartDialogue(const int32 InIndex, const bool bShowPressAnyKeyToContinue, const float InSpeed)
{
	if (!DialogueTexts.IsValidIndex(InIndex) && InIndex != -1) return;
	if (GetVisibility() == ESlateVisibility::Collapsed) SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	bShowPressAnyKeyToContinueOnDialogueEnd = bShowPressAnyKeyToContinue;
	AssetsManager->PlaySound2D(ESound::DialogueStart);
	DialogueTextIndex = InIndex;
	TargetText = DialogueTextIndex == -1 ? TipTexts[FMath::RandRange(0, TipTexts.Num() - 1)] : DialogueTexts[InIndex];
	CurrentText = "";
	CurrentIndex = 0;
	TypeSpeed = InSpeed;
	GetWorld()->GetTimerManager().SetTimer(TypeHandle, this, &UDialogueUI::TypeNextLetter, TypeSpeed, true);
}

void UDialogueUI::NativeConstruct()
{
	Super::NativeConstruct();
	DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	AssetsManager = GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>();
    if (DialogueText) DialogueText->SetText(FText::FromString(""));
}

void UDialogueUI::FinishedTypingDialogue()
{
	GetWorld()->GetTimerManager().ClearTimer(TypeHandle);
	if (bShowPressAnyKeyToContinueOnDialogueEnd)
	{
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnShowPressKeyToContinue.Broadcast(true);
		bShowPressAnyKeyToContinueOnDialogueEnd = false;
	}
}

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

#include "DialogueUI.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/TutorialManager.h"

void UDialogueUI::ClearDialogueText() const { if (DialogueText) DialogueText->SetText(FText::FromString("")); }

void UDialogueUI::StartDialogue(const int32 InIndex, const bool bShowPressAnyKeyToContinue, const float InSpeed)
{
	if (!DialogueTexts.IsValidIndex(InIndex) && InIndex != -1) return;
	if (GetVisibility() == ESlateVisibility::Collapsed) SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	bShowPressAnyKeyToContinueOnDialogueEnd = bShowPressAnyKeyToContinue;
	AssetsManager->PlaySound2D(ESound::DialogueStart);
	DialogueTextIndex = InIndex;
	TargetText = (DialogueTextIndex == -1 ? TipTexts[FMath::RandRange(0, TipTexts.Num() - 1)] : DialogueTexts[InIndex]).ToString();
	CurrentText = "";
	CurrentIndex = 0;
	TypeSpeed = InSpeed;
	GetWorld()->GetTimerManager().SetTimer(TypeHandle, this, &UDialogueUI::TypeNextLetter, TypeSpeed, true);
}

void UDialogueUI::StartTipDialogue(const float InSpeed)
{
	if (GetVisibility() == ESlateVisibility::Collapsed) SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	AssetsManager->PlaySound2D(ESound::DialogueStart);
	TargetText = TipTexts[FMath::RandRange(0, TipTexts.Num() - 1)].ToString();
	CurrentText = "";
	CurrentIndex = 0;
	TypeSpeed = InSpeed;
	GetWorld()->GetTimerManager().SetTimer(TypeHandle, this, &UDialogueUI::TypeNextLetter, TypeSpeed, true);
}

void UDialogueUI::NativeConstruct()
{
	Super::NativeConstruct();
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue0", "Owch."));
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue1", "Hi! My name is Bot, I'm here to get you out of here."));
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue2", "Ok, let's get you a body first."));
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue3", "See this body? Look at it and hijack it!"));
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue4", "Nice!"));
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue5", "Damn it!"));
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue6", "It's not safe for us here."));
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue7", "Down the hall further is a core room we're we can safely keep as our base."));
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue8", "I'll go through the vents, you follow the path."));
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue9", "Break these doors and get going."));
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue10", "We're almost there. Keep going!"));
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue11", "You made it!"));
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue12", "Now let's set everything up."));
	TipTexts.Add(NSLOCTEXT("ST_Localization", "Tip0", "There's someone working with us behind the scenes, wonder who."));
	TipTexts.Add(NSLOCTEXT("ST_Localization", "Tip1", "Well done, keep going!"));
	TipTexts.Add(NSLOCTEXT("ST_Localization", "Tip2", "Nice work, you're pretty good at this."));
	TipTexts.Add(NSLOCTEXT("ST_Localization", "Tip3", "My radars sense we have an ally, just can't seem to find him."));
	TipTexts.Add(NSLOCTEXT("ST_Localization", "Tip4", "Your Hijack bar charges with each kill, use this to your advantage."));
	TipTexts.Add(NSLOCTEXT("ST_Localization", "Tip5", "Good shooting, just don't forget to check your ammo sometimes."));
	TipTexts.Add(NSLOCTEXT("ST_Localization", "Tip6", "Must be lucky to be with infinite lives right?"));
	TipTexts.Add(NSLOCTEXT("ST_Localization", "Tip7", "Choose upgrades carefuly! So the enemy doesin't become too strong."));
	TipTexts.Add(NSLOCTEXT("ST_Localization", "Tip8", "Let's setup your abilities before you go."));
	TipTexts.Add(NSLOCTEXT("ST_Localization", "Tip9", "Did you setup your abilities?"));
	TipTexts.Add(NSLOCTEXT("ST_Localization", "Tip10", "Have a look at your abilities before you go."));
	DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	AssetsManager = GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>();
    if (DialogueText) DialogueText->SetText(FText::FromString(""));
}

void UDialogueUI::FinishedTypingDialogue()
{
	GetWorld()->GetTimerManager().ClearTimer(TypeHandle);
	if (bShowPressAnyKeyToContinueOnDialogueEnd)
	{
		GetWorld()->GetSubsystem<UTutorialManager>()->OnShowPressKeyToContinue.Broadcast(true);
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

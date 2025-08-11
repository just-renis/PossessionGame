#include "TutorialDialogue.h"
#include "Components/WidgetComponent.h"
#include "PossessionGame/UI/SecondaryWidgets/DialogueUI.h"

void ATutorialDialogue::SetVisibilityState(const bool bState) const
{
	Dialogue->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	if (!bState) Dialogue->ClearDialogueText();
}

ATutorialDialogue::ATutorialDialogue()
{
	DialogueWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("dialogueWidgetComponent");
	RootComponent = DialogueWidgetComponent;
}

void ATutorialDialogue::BeginPlay()
{
	Super::BeginPlay();
	Dialogue = Cast<UDialogueUI>(DialogueWidgetComponent->GetWidget());
}

void ATutorialDialogue::StartDialogue(const int32 InIndex, const bool bShowPressAnyKeyToContinue) const
{
	if (!Dialogue) return;
	SetVisibilityState(true);
	Dialogue->StartDialogue(InIndex, bShowPressAnyKeyToContinue);
}

void ATutorialDialogue::StartTipDialogue() const
{
	if (!Dialogue) return;
	SetVisibilityState(true);
	Dialogue->StartTipDialogue();
}

void ATutorialDialogue::StopShowingDialogue() const
{
	if (!Dialogue) return;
	SetVisibilityState(false);
}

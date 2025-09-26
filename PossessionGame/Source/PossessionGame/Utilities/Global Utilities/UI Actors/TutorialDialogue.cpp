#include "TutorialDialogue.h"
#include "Components/WidgetComponent.h"
#include "PossessionGame/Enums/EGameState.h"
#include "PossessionGame/Enums/ETutorialBotExpression.h"
#include "PossessionGame/UI/SecondaryWidgets/DialogueUI.h"

ETutorialBotExpression ATutorialDialogue::StartTipDialogue(const EGameState InGameState) const
{
	if (!Dialogue) return ETutorialBotExpression::NeutralFace;
	SetVisibilityState(true);
	return Dialogue->StartTipDialogue(InGameState);
}

void ATutorialDialogue::SetVisibilityState(const bool bState) const
{
	Dialogue->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	if (!bState)
	{
		Dialogue->ClearDialogueText();
		DialogueWidgetComponent->SetVisibility(false, true);
		DialogueWidgetComponent->SetRedrawTime(0.f);
	}
	else
	{
		DialogueWidgetComponent->SetVisibility(true, true);
		DialogueWidgetComponent->RequestRedraw();
	}
}

void ATutorialDialogue::StartDialogue(const int32 InIndex, const bool bShowPressAnyKeyToContinue) const
{
	if (!Dialogue) return;
	SetVisibilityState(true);
	Dialogue->StartDialogue(InIndex, bShowPressAnyKeyToContinue);
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

void ATutorialDialogue::StopShowingDialogue() const
{
	if (!Dialogue) return;
	SetVisibilityState(false);
}

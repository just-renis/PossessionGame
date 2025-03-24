#include "TutorialDialogue.h"
#include "Components/WidgetComponent.h"
#include "PossessionGame/UI/SecondaryWidgets/DialogueUI.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

ATutorialDialogue::ATutorialDialogue()
{
	DialogueWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("dialogueWidgetComponent");
	RootComponent = DialogueWidgetComponent;
}

void ATutorialDialogue::BeginPlay()
{
	Super::BeginPlay();
	Dialogue = Cast<UDialogueUI>(DialogueWidgetComponent->GetWidget());
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnShowDialogue.AddUObject(this, &ATutorialDialogue::StartDialogue);
}

void ATutorialDialogue::StartDialogue(const int32 InIndex) const
{
	if (Dialogue)
	{
		Dialogue->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Dialogue->StartDialogue(InIndex);
	}
}
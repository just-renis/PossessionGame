#include "TutorialDialogue.h"
#include "Components/WidgetComponent.h"
#include "PossessionGame/UI/SecondaryWidgets/TutorialDialogueUI.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

void ATutorialDialogue::SetVisibilityState(const bool bState) const
{
	Dialogue->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

ATutorialDialogue::ATutorialDialogue()
{
	DialogueWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("dialogueWidgetComponent");
	RootComponent = DialogueWidgetComponent;
}

void ATutorialDialogue::BeginPlay()
{
	Super::BeginPlay();
	Dialogue = Cast<UTutorialDialogueUI>(DialogueWidgetComponent->GetWidget());
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnShowDialogue.BindUObject(this, &ATutorialDialogue::StartDialogue);
}

void ATutorialDialogue::StartDialogue(const int32 InIndex) const
{
	if (Dialogue)
	{
		Dialogue->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Dialogue->StartDialogue(InIndex);
	}
}
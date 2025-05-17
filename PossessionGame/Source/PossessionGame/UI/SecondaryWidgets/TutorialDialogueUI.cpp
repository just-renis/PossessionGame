#include "TutorialDialogueUI.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

void UTutorialDialogueUI::ClearDialogueText() const { if (DialogueText) DialogueText->SetText(FText::FromString("")); }

void UTutorialDialogueUI::FinishedTypingDialogue()
{
	Super::FinishedTypingDialogue();
	switch (DialogueTextIndex)
	{
		case 0: DelegateManager->OnShowPressKeyToContinue.Broadcast(true); break;
		case 1:
			StartGuardianSpawn();
			DelegateManager->OnShowPressKeyToContinue.Broadcast(true);
			break;
		case 3: DelegateManager->OnShowPressKeyToContinue.Broadcast(true); break;
		case 5: DelegateManager->OnShowPressKeyToContinue.Broadcast(true); break;
		case 6: DelegateManager->OnShowPressKeyToContinue.Broadcast(true); break;
		default: ;
	}
}

void UTutorialDialogueUI::StartGuardianSpawn() const { (void)DelegateManager->SpawnTutorialEnemy.ExecuteIfBound(SpawnWhat); }

#include "TutorialDialogueUI.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

void UTutorialDialogueUI::FinishedTypingDialogue()
{
	Super::FinishedTypingDialogue();
	switch (DialogueTextIndex)
	{
		case 0: (void)DelegateManager->OnShowPressAnyKey.ExecuteIfBound(true); break;
		case 1: (void)DelegateManager->SpawnTutorialGuardian.ExecuteIfBound(ECharacterType::Guardian); break;
		default: ;
	}
}

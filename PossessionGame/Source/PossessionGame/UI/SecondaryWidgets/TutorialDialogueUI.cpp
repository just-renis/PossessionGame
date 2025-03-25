#include "TutorialDialogueUI.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

void UTutorialDialogueUI::FinishedTypingDialogue()
{
	Super::FinishedTypingDialogue();
	switch (DialogueTextIndex)
	{
		case 0: (void)DelegateManager->OnShowPressAnyKey.ExecuteIfBound(true); break;
		case 1: StartGuardianSpawn(); break;
		default: ;
	}
}

void UTutorialDialogueUI::StartGuardianSpawn()
{
	(void)DelegateManager->SpawnTutorialGuardian.ExecuteIfBound(ECharacterType::Guardian);
	DelegateManager->OnShowDialogue.Broadcast(2);
	DelegateManager->OnShowHint.Broadcast(1);
}

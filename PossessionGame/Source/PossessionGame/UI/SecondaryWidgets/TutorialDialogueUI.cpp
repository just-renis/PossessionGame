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
		case 3: (void)DelegateManager->OnShowPressAnyKey.ExecuteIfBound(true); break;
		case 5: (void)DelegateManager->OpenTutorialGate.ExecuteIfBound(); break;
		default: ;
	}
}

void UTutorialDialogueUI::StartGuardianSpawn()
{
	(void)DelegateManager->SpawnTutorialEnemy.ExecuteIfBound(ECharacterType::Guardian);
	(void)DelegateManager->OnShowDialogue.ExecuteIfBound(2);
	(void)DelegateManager->OnShowHint.ExecuteIfBound(1);
}

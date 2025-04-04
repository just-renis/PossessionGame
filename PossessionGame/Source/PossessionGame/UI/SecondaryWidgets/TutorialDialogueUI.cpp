#include "TutorialDialogueUI.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

void UTutorialDialogueUI::ClearDialogueText() const { if (DialogueText) DialogueText->SetText(FText::FromString("")); }

void UTutorialDialogueUI::FinishedTypingDialogue()
{
	Super::FinishedTypingDialogue();
	switch (DialogueTextIndex)
	{
		case 0: (void)DelegateManager->OnShowPressAnyKey.ExecuteIfBound(true); break;
		case 1:
			StartGuardianSpawn();
			(void)DelegateManager->OnShowPressAnyKey.ExecuteIfBound(true);
			break;
		case 3:
			(void)DelegateManager->OnShowPressAnyKey.ExecuteIfBound(true);
			SetVisibility(ESlateVisibility::Collapsed);
			ClearDialogueText();
			break;
		case 5: (void)DelegateManager->OnShowPressAnyKey.ExecuteIfBound(true); break;
		case 6: (void)DelegateManager->OnShowPressAnyKey.ExecuteIfBound(true); break;
		default: ;
	}
}

void UTutorialDialogueUI::StartGuardianSpawn()
{
	(void)DelegateManager->SpawnTutorialEnemy.ExecuteIfBound(ECharacterType::Guardian);
	(void)DelegateManager->OnShowHint.ExecuteIfBound(1);
}

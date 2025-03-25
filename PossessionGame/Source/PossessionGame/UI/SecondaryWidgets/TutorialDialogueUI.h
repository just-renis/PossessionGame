#pragma once

#include "CoreMinimal.h"
#include "DialogueUI.h"
#include "TutorialDialogueUI.generated.h"

UCLASS()
class POSSESSIONGAME_API UTutorialDialogueUI : public UDialogueUI
{
	GENERATED_BODY()

	virtual void FinishedTypingDialogue() override;
};

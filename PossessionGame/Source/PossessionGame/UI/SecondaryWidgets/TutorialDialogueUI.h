#pragma once

#include "CoreMinimal.h"
#include "DialogueUI.h"
#include "TutorialDialogueUI.generated.h"

enum class ECharacterType : uint8;

UCLASS()
class POSSESSIONGAME_API UTutorialDialogueUI : public UDialogueUI
{
	GENERATED_BODY()
public:
	void ClearDialogueText() const;
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	ECharacterType SpawnWhat;
	
	virtual void FinishedTypingDialogue() override;
	void StartGuardianSpawn() const;
};

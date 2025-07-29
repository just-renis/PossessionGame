#pragma once

#include "CoreMinimal.h"
#include "TutorialDialogue.generated.h"

class UDialogueUI;
class UWidgetComponent;

UCLASS()
class POSSESSIONGAME_API ATutorialDialogue : public AActor
{
	GENERATED_BODY()
public:
	void SetVisibilityState(const bool bState) const;
	void StartDialogue(const int32 InIndex) const;
private:
	ATutorialDialogue();
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> DialogueWidgetComponent;
	UPROPERTY()
	TObjectPtr<UDialogueUI> Dialogue;

	virtual void BeginPlay() override;
	void StopShowingDialogue() const;
};

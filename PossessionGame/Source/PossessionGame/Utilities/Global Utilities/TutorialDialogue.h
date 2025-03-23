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
	void StartDialogue(const FString& InFullText, const float InSpeed = 0.05f) const;
private:
	ATutorialDialogue();
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> DialogueWidgetComponent;
	UPROPERTY()
	TObjectPtr<UDialogueUI> Dialogue;

	virtual void BeginPlay() override;
};

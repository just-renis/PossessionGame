#pragma once

#include "CoreMinimal.h"
#include "TutorialDialogue.generated.h"

enum class ETutorialBotExpression : uint8;
enum class EGameState : uint8;
class UDialogueUI;
class UWidgetComponent;

UCLASS()
class POSSESSIONGAME_API ATutorialDialogue : public AActor
{
	GENERATED_BODY()
public:
	void StartTipDialogue(const EGameState InGameState) const;
	void SetVisibilityState(const bool bState) const;
	void StartDialogue(const int32 InIndex, const bool bShowPressAnyKeyToContinue) const;
	void SetNextDialogueText(const EGameState InGameState) const;
private:
	ATutorialDialogue();
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> DialogueWidgetComponent;
	UPROPERTY()
	TObjectPtr<UDialogueUI> Dialogue;

	virtual void BeginPlay() override;
	void StopShowingDialogue() const;
};

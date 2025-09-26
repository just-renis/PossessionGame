#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueUI.generated.h"

enum class ETutorialBotAnimState : uint8;
enum class EGameState : uint8;
enum class ECharacterType : uint8;
enum class ETutorialBotExpression : uint8;
class UPrimaryDataAssetsManager;
class UDelegateManager;
class UTextBlock;

USTRUCT()
struct FExpressionDialogues
{
	GENERATED_BODY()

	ETutorialBotAnimState Animation;
	UPROPERTY()
	TArray<FText> Texts;
};

USTRUCT()
struct FGameStateDialogues
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<ETutorialBotExpression, FExpressionDialogues> Texts;
};

UCLASS()
class POSSESSIONGAME_API UDialogueUI : public UUserWidget
{
	GENERATED_BODY()
public:
	ETutorialBotExpression StartTipDialogue(const EGameState InCurrentGameState, const float InSpeed = 0.025f);
	
	void ClearDialogueText() const;
	void StartDialogue(const int32 InIndex, const bool bShowPressAnyKeyToContinue, const float InSpeed = 0.025f);
protected:
	bool bShowPressAnyKeyToContinueOnDialogueEnd = false;
	UPROPERTY()
	TArray<FText> DialogueTexts;
	UPROPERTY()
	TMap<EGameState, FGameStateDialogues> TipTexts;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DialogueText;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;
	UPROPERTY()
	FTimerHandle TypeHandle;
	
	void FinishedTypingDialogue();
private:
	int32 CurrentIndex = 0;
	float TypeSpeed = 0.05f;
	FString TargetText;
	FString CurrentText;
	ETutorialBotExpression SavedExpression;
	UPROPERTY()
	TObjectPtr<UPrimaryDataAssetsManager> AssetsManager;

	FText GetGameStateDialogues(const EGameState InGameState);

	virtual void NativeConstruct() override;
	void TypeNextLetter();
};

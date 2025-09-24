#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueUI.generated.h"

enum class ECharacterType : uint8;
class UPrimaryDataAssetsManager;
class UDelegateManager;
class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UDialogueUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void ClearDialogueText() const;
	void StartDialogue(const int32 InIndex, const bool bShowPressAnyKeyToContinue, const float InSpeed = 0.025f);
	void StartTipDialogue(const float InSpeed = 0.025f);
protected:
	bool bShowPressAnyKeyToContinueOnDialogueEnd = false;
	int32 DialogueTextIndex = -1;
	UPROPERTY()
	TArray<FText> DialogueTexts;
	UPROPERTY()
	TArray<FText> TipTexts;
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
	UPROPERTY()
	TObjectPtr<UPrimaryDataAssetsManager> AssetsManager;

	virtual void NativeConstruct() override;
	void TypeNextLetter();
};

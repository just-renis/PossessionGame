#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueUI.generated.h"

class UPrimaryDataAssetsManager;
class UDelegateManager;
class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UDialogueUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void StartDialogue(const int32 InIndex, const float InSpeed = 0.01f);
protected:
	int32 DialogueTextIndex = -1;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;
	
	virtual void FinishedTypingDialogue();
private:
	int32 CurrentIndex = 0;
	float TypeSpeed = 0.05f;
	FString TargetText;
	FString CurrentText;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TArray<FString> DialogueTexts;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DialogueText;
	UPROPERTY()
	TObjectPtr<UPrimaryDataAssetsManager> AssetsManager;

	virtual void NativeConstruct() override;
	UFUNCTION()
	void TypeNextLetter();
};

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerGhostUI.generated.h"

class UDelegateManager;
class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UPlayerGhostUI : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HintText;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TArray<FString> HintTexts;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;

	virtual void NativeConstruct() override;
	void ShowHint(const int32 InIndex);
	void SetHintText(const FString& InText) const;
};

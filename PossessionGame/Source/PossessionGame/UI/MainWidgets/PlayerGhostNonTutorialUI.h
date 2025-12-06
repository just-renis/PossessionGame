#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerGhostNonTutorialUI.generated.h"

class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UPlayerGhostNonTutorialUI : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HijackHintText;

	virtual void NativeConstruct() override;
	void UpdateHijackKeyBindText(const FName& InKeyBindName, const FText& InKeyBindText) const;
	virtual void NativeDestruct() override;
	void DestroyThis() const;
};

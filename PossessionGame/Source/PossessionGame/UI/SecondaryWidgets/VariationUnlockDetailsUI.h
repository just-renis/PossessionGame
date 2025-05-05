#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VariationUnlockDetailsUI.generated.h"

class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UVariationUnlockDetailsUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetDetailsText(const FText& InText) const;
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DetailsText;
};

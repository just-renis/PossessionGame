#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VariationUnlockDetailsUI.generated.h"

class UCheckBox;
class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UVariationUnlockDetailsUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetDetailsText(const FText& InText) const;
private:
	UPROPERTY(EditAnywhere, Category = "Details Panel", meta = (AllowPrivateAccess = true))
	bool bIsCheckBoxNeeded = false;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCheckBox> DetailsCheck;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DetailsText;

	virtual void NativePreConstruct() override;
};

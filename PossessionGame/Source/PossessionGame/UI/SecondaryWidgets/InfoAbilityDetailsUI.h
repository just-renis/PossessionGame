#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InfoAbilityDetailsUI.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UInfoAbilityDetailsUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetAbilityIcon(const TObjectPtr<UTexture2D>& InAbilityIcon) const;
	void SetText(const FText& InText) const;
	void SetAbilityVisibility(const bool bState) const;
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> AbilityIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AbilityExplanationText;
};

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VariationDescriptionUI.generated.h"

class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UVariationDescriptionUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetDetailsText(const FText& InText) const;
	void PlayFadeInAnim() { PlayAnimation(FadeInAnim); }
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DetailsText;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnim;
};

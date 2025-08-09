#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ExplanationHighlight.generated.h"

class UImage;

UCLASS()
class POSSESSIONGAME_API UExplanationHighlight : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetTexture(const TObjectPtr<UTexture2D>& InTexture) const;
	void PlayFadeAnim(const bool bState);
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ExplanationHighlight;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeOutAnim;
};

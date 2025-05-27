#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CustomTextUI.generated.h"

class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UCustomTextUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetText(const FText& InText) const;
	void PlayFadeInAnim(const bool bState);
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CustomText;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnim;
	UPROPERTY()
	FTimerHandle FadeInHandle;

	void FadeInAnimFinished();
};

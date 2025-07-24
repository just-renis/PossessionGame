#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CustomTextUI.generated.h"

class UBorder;
class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UCustomTextUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetText(const FText& InText) const;
	void PlayFadeInAnim(const bool bState);
private:
	UPROPERTY(EditAnywhere, Category = "Background Image", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> BackgroundTexture;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CustomText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> ImageBorder;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnim;

	virtual void NativePreConstruct() override;
};

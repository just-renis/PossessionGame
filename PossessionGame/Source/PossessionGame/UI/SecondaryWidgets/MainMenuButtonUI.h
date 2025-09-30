#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuButtonUI.generated.h"

class UBorder;
class UTextBlock;
class UButton;

UCLASS()
class POSSESSIONGAME_API UMainMenuButtonUI : public UUserWidget
{
	GENERATED_BODY()
public:
	TObjectPtr<UButton> Get() { return Button; }
	
	void SetText(const FText& InText) const;
	void SetButtonVisibility(const bool bState);
	void SetFontSize(const int32 InFontSize) const;
private:
	UPROPERTY(EditAnywhere, Category = "Button Sfx", meta = (AllowPrivateAccess = true))
	bool bStrongButtonSelect = true;
	UPROPERTY(EditAnywhere, Category = "Button Text", meta = (AllowPrivateAccess = true))
	int32 FontSize;
	UPROPERTY(EditAnywhere, Category = "Button Text", meta = (AllowPrivateAccess = true))
	FText Text;
	UPROPERTY(EditAnywhere, Category = "Button Background", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> BackgroundTexture;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ButtonText;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ButtonHoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ButtonClickAnim;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_ButtonBackground;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	void SetupButtonEvents();
	void SetupDynamicMaterialForButton();
	UFUNCTION()
	void TriggerClickAnim();
	UFUNCTION()
	void TriggerHoverAnim();
	UFUNCTION()
	void TriggerUnHoverAnim();
};

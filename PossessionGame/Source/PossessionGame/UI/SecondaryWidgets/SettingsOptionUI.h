#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsOptionUI.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class POSSESSIONGAME_API USettingsOptionUI : public UUserWidget
{
	GENERATED_BODY()
public:
	TObjectPtr<UButton> GetDownscaleButton() { return DownscaleButton; }
	TObjectPtr<UButton> GetUpscaleButton() { return UpscaleButton; }

	void SetText(const FText& InText) const;
protected:
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> Text;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> DownscaleButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> UpscaleButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> LeftArrowClickAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> RightArrowClickAnim;
	
	virtual void NativeConstruct() override;
	UFUNCTION()
	virtual void OnLeftArrowClicked();
	UFUNCTION()
	virtual void OnRightArrowClicked();
private:
	UPROPERTY(EditAnywhere, Category = "Text Font Size", meta = (AllowPrivateAccess = true))
	int32 FontSize = 20;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> LeftArrowHoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> RightArrowHoverAnim;

	virtual void NativePreConstruct() override;
	void SetupButtonEvents();
	UFUNCTION()
	void OnLeftArrowHovered();
	UFUNCTION()
	void OnLeftArrowUnHovered();
	UFUNCTION()
	void OnRightArrowHovered();
	UFUNCTION()
	void OnRightArrowUnHovered();
};

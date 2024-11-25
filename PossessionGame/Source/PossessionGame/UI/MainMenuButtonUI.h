#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuButtonUI.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class POSSESSIONGAME_API UMainMenuButtonUI : public UUserWidget
{
	GENERATED_BODY()
public:
	TObjectPtr<UButton> Get() { return Button; }
	
	void SetupEvents();
	UFUNCTION()
	void TriggerHoverAnim();
private:
	bool ButtonHoverState = false;
	UPROPERTY(EditAnywhere, Category = "Button Text", meta = (AllowPrivateAccess = true))
	FText Text;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> Button;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> ButtonText;
	UPROPERTY(Transient, meta = (BindWidgetAnim, AllowPrivateAccess = true))
	TObjectPtr<UWidgetAnimation> ButtonHoverAnim;

	virtual void NativePreConstruct() override;
};

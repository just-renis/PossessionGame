#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "ArrowButtonUI.generated.h"

UCLASS()
class POSSESSIONGAME_API UArrowButtonUI : public UUserWidget
{
	GENERATED_BODY()
public:
	FOnButtonClickedEvent& GetClickEvent() const { return ArrowButton->OnClicked; }
	void SetupEvents();
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ArrowButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PressAnim;

	UFUNCTION()
	void PlayHoveredAnim();
	UFUNCTION()
	void PlayPressAnim();
	UFUNCTION()
	void PlayUnHoveredAnim();
};

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilityResetButtonUI.generated.h"

class UButton;

UCLASS()
class POSSESSIONGAME_API UAbilityResetButtonUI : public UUserWidget
{
	GENERATED_BODY()
public:
	TObjectPtr<UButton> GetButton() { return AbilityResetButton; }
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AbilityResetButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> SelectAnim;

	virtual void NativeConstruct() override;
	UFUNCTION()
	void OnHovered();
	UFUNCTION()
	void OnUnHovered();
	UFUNCTION()
	void OnSelect();
};

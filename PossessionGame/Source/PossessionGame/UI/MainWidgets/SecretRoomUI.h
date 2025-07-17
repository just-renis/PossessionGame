#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SecretRoomUI.generated.h"

class UCustomTextUI;
class USecretBoostCardUI;
class UAlertPopupUI;

UCLASS()
class POSSESSIONGAME_API USecretRoomUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void OpenUI();
	void CloseUI();
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCustomTextUI> BoostDetailsPanel;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USecretBoostCardUI> HpCard;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USecretBoostCardUI> DamageCard;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USecretBoostCardUI> XpCard;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeOutAnim;
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeOutAnimEndDelegate;

	virtual void NativeConstruct() override;
	void ToggleUI(const bool bState) const;
	void ToggleDetailsPanel(const bool bState, const FText& InText) const;
	UFUNCTION()
	void FadeOutAnimFinished();
};

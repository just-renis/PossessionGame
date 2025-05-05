#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VariationsUI.generated.h"

class UInvalidationBox;
class UVariationOptionUI;
class UVariationUnlockDetailsUI;
class UWidgetSwitcher;
class UVariationCharacterIconUI;

UCLASS()
class POSSESSIONGAME_API UVariationsUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void OpenUI();
	void CloseUI();
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationCharacterIconUI> GuardianCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationCharacterIconUI> RangerCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationCharacterIconUI> TricksterCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> VariationsSwitcher;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationUnlockDetailsUI> UnlockDetailsPanel;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> GuardianVariationDefault;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> GuardianVariationGold;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> RangerVariationDefault;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> RangerVariationBlue;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> TricksterVariationDefault;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> TricksterVariationPurple;

	virtual void NativeConstruct() override;
	void Toggle(const bool bState);
	UFUNCTION()
	void SwitchVariation(const int32 InIndex) const;
	UFUNCTION()
	void ToggleDetailsPanel(const bool bState, const FText& InDetailsText) const;
};

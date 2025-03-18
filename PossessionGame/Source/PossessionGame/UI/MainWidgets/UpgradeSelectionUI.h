#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpgradeSelectionUI.generated.h"

class UPossessionGameInstance;
class UUpgradeCardUI;

UCLASS()
class POSSESSIONGAME_API UUpgradeSelectionUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void OpenUI();
	void CloseUI();
private:
	UPROPERTY(EditDefaultsOnly, Category = "UI Assets", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> PlayerCardBackgroundTexture;
	UPROPERTY(EditDefaultsOnly, Category = "UI Assets", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> PlayerIconTexture;
	UPROPERTY(EditDefaultsOnly, Category = "UI Assets", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> EnemyIconTexture;
	UPROPERTY(EditDefaultsOnly, Category = "UI Assets", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> RangerIconTexture;
	UPROPERTY(EditDefaultsOnly, Category = "UI Assets", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> TricksterIconTexture;
	UPROPERTY(EditDefaultsOnly, Category = "UI Assets", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> EnemyCardBackgroundTexture;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UUpgradeCardUI> PlayerCardOption1;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UUpgradeCardUI> PlayerCardOption2;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UUpgradeCardUI> EnemyCardOption1;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UUpgradeCardUI> EnemyCardOption2;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PlayerCardOption1Popup;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PlayerCardOption2Popup;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> EnemyCardOption1Popup;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> EnemyCardOption2Popup;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PlayerCardOption1Select;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PlayerCardOption2Select;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> EnemyCardOption1Select;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> EnemyCardOption2Select;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> UpgradeMenuOpen;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> UpgradeMenuClose;
	
	void Toggle(const bool bState);
	virtual void NativeConstruct() override;
	void TriggerUpgradeMenuCloseAnimation();
	UFUNCTION()
	void FinishedUpgradeSelectionCloseAnimation() const;
	UFUNCTION()
	void TriggerHoverAnimForFirstOption();
	UFUNCTION()
	void TriggerHoverAnimForSecondOption();
	UFUNCTION()
	void TriggerSelectAnimForFirstOption();
	UFUNCTION()
	void TriggerSelectAnimForSecondOption();
};

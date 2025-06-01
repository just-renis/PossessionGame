#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpgradeSelectionUI.generated.h"

class UVerticalBox;
struct FUpgradeAttributes;
enum class EUpgrade : uint8;
enum class EUpgradeAttribute : uint8;
class UUpgradeData;
class UUpgradeDetailsPanelUI;
class UDelegateManager;
class UPossessionGameInstance;
class UUpgradeCardUI;

UCLASS()
class POSSESSIONGAME_API UUpgradeSelectionUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void OpenUI();
	void CloseUI();
	void TriggerUpgradeMenuCloseAnimation();
private:
	bool bFirstOptionSelected = false;
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
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUpgradeDetailsPanelUI> UpgradeChoice1;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUpgradeDetailsPanelUI> UpgradeChoice2;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUpgradeDetailsPanelUI> UpgradeChoice3;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> UpgradeChoicesPanel;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> UpgradeMenuOpenAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> UpgradeMenuCloseAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> UpgradeSelectFirstAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> UpgradeSelectSecondAnim;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;
	UPROPERTY()
	FWidgetAnimationDynamicEvent UpgradeSelectionFadeOutDelegate;
	UPROPERTY()
	TArray<TObjectPtr<UUpgradeData>> UpgradesData;

	FString GetUpgradeTextFromUpgrade(const EUpgrade InUpgrade);
	
	void Toggle(const bool bState);
	virtual void NativeConstruct() override;
	void OpenUpgradeChoicesPanel(const TObjectPtr<UUpgradeData>& InUpgradeData) const;
	UFUNCTION()
	void TriggerHoverAnimForFirstOption();
	UFUNCTION()
	void TriggerUnHoverAnimForFirstOption();
	UFUNCTION()
	void TriggerHoverAnimForSecondOption();
	UFUNCTION()
	void TriggerUnHoverAnimForSecondOption();
	UFUNCTION()
	void TriggerSelectAnimForFirstOption();
	UFUNCTION()
	void TriggerSelectAnimForSecondOption();
	UFUNCTION()
	void UpgradeSelectionPanelFadeOutFinished();
};

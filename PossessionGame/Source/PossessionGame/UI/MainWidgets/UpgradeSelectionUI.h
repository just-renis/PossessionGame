#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpgradeSelectionUI.generated.h"

enum class EUpgrade : uint8;
enum class EUpgradeAttribute : uint8;
struct FLevelUpgradeData;
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
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUpgradeDetailsPanelUI> UpgradeDetailsPanel;
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
	TObjectPtr<UWidgetAnimation> PlayerCardOption1UnHover;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PlayerCardOption2UnHover;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> EnemyCardOption1UnHover;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> EnemyCardOption2UnHover;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> UpgradeMenuOpen;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> UpgradeMenuClose;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;
	UPROPERTY()
	TArray<TObjectPtr<UUpgradeData>> UpgradesData;

	FString ConvertUpgradeAttributeValueToString(const float InValue);
	FString GetUpgradeTextFromUpgrade(const EUpgrade InUpgrade);
	FString GetUpgradeTextFromSingularAttributeValue(const TPair<EUpgradeAttribute, float> InAttribute);
	FString ReceiveUpgradeText(const bool bPlayerCard, const TObjectPtr<UUpgradeData>& InUpgradeData, const bool bIsForDetailsPanel = false);
	
	void Toggle(const bool bState);
	virtual void NativeConstruct() override;
	void SetTriggerUpgradeMenuCloseAnimation();
	void ProcessCurrentUpgradeData(const bool bPlayerCard, const int32 PreviousLevel, const TObjectPtr<UUpgradeData>& InUpgradeData, FLevelUpgradeData& CurrentUpgradeData);
	UFUNCTION()
	void FinishedUpgradeSelectionCloseAnimation() const;
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
	void TriggerUpgradeMenuCloseAnimation();
};

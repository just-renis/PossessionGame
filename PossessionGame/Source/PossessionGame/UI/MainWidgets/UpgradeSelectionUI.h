#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpgradeSelectionUI.generated.h"

enum class ECharacterType : uint8;
class UMainMenuButtonUI;
class UVerticalBox;
enum class EUpgrade : uint8;
class UUpgradeData;
class UUpgradeDetailsPanelUI;
class UDelegateManager;
class UUpgradeCardUI;

UCLASS()
class POSSESSIONGAME_API UUpgradeSelectionUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void OpenUI();
	void CloseUI();
	void TriggerUpgradeMenuCloseAnimation();
	UFUNCTION()
	void GoBack();
private:
	bool bChoicesOpen = false;
	int32 OptionSelected = 0;
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
	TObjectPtr<UTexture2D> GuardianIconTexture;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UUpgradeCardUI> CardOption1;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UUpgradeCardUI> CardOption2;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UUpgradeCardUI> CardOption3;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UUpgradeCardUI> CardOption4;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUpgradeDetailsPanelUI> UpgradeChoice1;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUpgradeDetailsPanelUI> UpgradeChoice2;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUpgradeDetailsPanelUI> UpgradeChoice3;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> UpgradeChoicesPanel;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> BackButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> UpgradeMenuOpenAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> UpgradeMenuCloseAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> UpgradeSelectFirstAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> UpgradeSelectSecondAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> UpgradeSelectThirdAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> UpgradeSelectFourthAnim;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;
	UPROPERTY()
	FWidgetAnimationDynamicEvent UpgradeSelectionFadeOutDelegate;
	UPROPERTY()
	FWidgetAnimationDynamicEvent GoBackAnimFinishedDelegate;
	UPROPERTY()
	TArray<TObjectPtr<UUpgradeData>> UpgradesData;
	UPROPERTY()
	TArray<TObjectPtr<UUpgradeCardUI>> CardOptions;

	FText GetUpgradeTextFromUpgrade(const EUpgrade InUpgrade);
	TObjectPtr<UTexture2D>& GetCharacterUpgradeIcon(const ECharacterType InCharacter);
	
	void Toggle(const bool bState);
	virtual void NativeConstruct() override;
	void OpenUpgradeChoicesPanel(const TObjectPtr<UUpgradeData>& InUpgradeData);
	void PlayAnimByOptionSelected(const bool bIncludeBindToFinish);
	UFUNCTION()
	void TriggerSelectAnimForFirstOption();
	UFUNCTION()
	void TriggerSelectAnimForSecondOption();
	UFUNCTION()
	void TriggerSelectAnimForThirdOption();
	UFUNCTION()
	void TriggerSelectAnimForFourthOption();
	UFUNCTION()
	void UpgradeSelectionPanelFadeOutFinished();
	UFUNCTION()
	void GoBackAnimFinished();
};

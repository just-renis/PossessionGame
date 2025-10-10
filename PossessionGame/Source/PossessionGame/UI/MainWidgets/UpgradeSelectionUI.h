#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpgradeSelectionUI.generated.h"

struct FRuntimeUpgradeData;
struct FUpgradeChoicePair;
enum class EUpgradeChoice : uint8;
enum class ECharacterType : uint8;
class UMainMenuButtonUI;
class UVerticalBox;
enum class EUpgrade : uint8;
class UUpgradeData;
class UUpgradeDetailsPanelUI;
class UDelegateManager;
class UUpgradeCardUI;

USTRUCT()
struct FCachedPairs
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FUpgradeChoicePair> Pairs;

	bool IsValidIndex(const int32 InIndex) const { return Pairs.IsValidIndex(InIndex); }
	int32 Num() const { return Pairs.Num(); }
};

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
	TObjectPtr<UTexture2D> RangerIconTexture;
	UPROPERTY(EditDefaultsOnly, Category = "UI Assets", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> TricksterIconTexture;
	UPROPERTY(EditDefaultsOnly, Category = "UI Assets", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> GuardianIconTexture;
	UPROPERTY(EditDefaultsOnly, Category = "UI Assets", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> BreacherIconTexture;
	UPROPERTY(EditDefaultsOnly, Category = "UI Assets", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> WraithIconTexture;
	UPROPERTY(EditDefaultsOnly, Category = "UI Assets", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> TankIconTexture;
	UPROPERTY(EditDefaultsOnly, Category = "UI Assets", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> SpyIconTexture;
	UPROPERTY(EditDefaultsOnly, Category = "UI Assets", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> SamuraiIconTexture;
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
	TArray<FRuntimeUpgradeData> UpgradesData;
	UPROPERTY()
	TArray<TObjectPtr<UUpgradeCardUI>> CardOptions;
	UPROPERTY()
	TMap<EUpgrade, FCachedPairs> CachedPairs;

	FText GetUpgradeTextFromUpgrade(const EUpgrade InUpgrade);
	TObjectPtr<UTexture2D>& GetCharacterUpgradeIcon(const ECharacterType InCharacter);
	
	void Toggle(const bool bState);
	virtual void NativeConstruct() override;
	void OpenUpgradeChoicesPanel(const FRuntimeUpgradeData& InUpgradeData);
	void PlayAnimByOptionSelected(const bool bIncludeBindToFinish);
	void AdjustNeededCounts(int32& InNeededCommon, int32& InNeededRare, int32& InNeededLegendary, const FRuntimeUpgradeData& InUpgradeData);
	void FillUpgrades(const FRuntimeUpgradeData& InUpgradeData, TArray<FUpgradeChoicePair>& OutChosen, const EUpgradeChoice InRarity, const int32 InCount);
	void SetupUpgradeChoices(const FRuntimeUpgradeData& InUpgradeData);
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

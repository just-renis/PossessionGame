#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "UpgradeDetailsPanelUI.generated.h"

class URichTextBlock;
class UImage;
enum class EUpgradeAttribute : uint8;
class UButton;

UCLASS()
class POSSESSIONGAME_API UUpgradeDetailsPanelUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetButtonVisibility(const bool bState) const;
	void Setup(const EUpgrade InUpgradeName, const FUpgradeChoicePair& InUpgradeChoicePair = FUpgradeChoicePair());
	UFUNCTION()
	void PlayDisappearAnim();
private:
	bool bHasBeenSelected = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 ChoiceIndex;
	UPROPERTY(EditDefaultsOnly, Category = "Textures", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UTexture2D>> Textures;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> PlayerUpgradeText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> EnemyUpgradeText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> UpgradeChoiceButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> UpgradeDetailsPanelOverlay;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> UpgradeChoiceBackground;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PressAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> UnHoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> DisappearAnim;
	UPROPERTY()
	EUpgrade UpgradeName;
	UPROPERTY()
	FUpgradeChoicePair UpgradeChoice;

	FString ConvertUpgradeAttributeValueToString(const float InValue);
	FString GetUpgradeTextFromSingularAttributeValue(const TPair<EUpgradeAttribute, float> InAttribute, const bool bForEnemy);
	FString GetRichTextTag(const bool bForEnemy) const;

	virtual void NativeConstruct() override;
	void SetUpgradeText(const bool bIsPlayerUpgrade);
	void SetUpgradeChoiceBackground();
	UFUNCTION()
	void SelectUpgrade();
	UFUNCTION()
	void PlayHoverAnim();
	UFUNCTION()
	void PlayUnHoverAnim();
};

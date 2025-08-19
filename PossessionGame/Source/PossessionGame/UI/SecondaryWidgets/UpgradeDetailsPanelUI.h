#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "UpgradeDetailsPanelUI.generated.h"

class UImage;
enum class EUpgradeAttribute : uint8;
struct FUpgradeAttributes;
class UUpgradeData;
class UTextBlock;
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
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerUpgradeText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EnemyUpgradeText;
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
	FString GetUpgradeTextFromSingularAttributeValue(const TPair<EUpgradeAttribute, float> InAttribute);

	virtual void NativeConstruct() override;
	void SetUpgradeText(const bool bIsPlayerUpgrade);
	UFUNCTION()
	void SelectUpgrade();
	UFUNCTION()
	void PlayHoverAnim();
	UFUNCTION()
	void PlayUnHoverAnim();
};

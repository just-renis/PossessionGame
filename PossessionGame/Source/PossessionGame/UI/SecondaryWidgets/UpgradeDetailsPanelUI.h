#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpgradeDetailsPanelUI.generated.h"

class UOverlay;
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
	void Setup(const TObjectPtr<UUpgradeData>& InUpgradeData);
	UFUNCTION()
	void PlayDisappearAnim() { if (!bHasBeenSelected) PlayAnimation(DisappearAnim); }
private:
	bool bHasBeenSelected = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 ChoiceIndex;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UpgradeText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> UpgradeChoiceButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> UpgradeChoiceLogo;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UpgradeChoiceIndexText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> UpgradeDetailsPanelOverlay;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PressAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> UnHoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> DisappearAnim;
	UPROPERTY()
	TObjectPtr<UUpgradeData> UpgradeData;

	FString ConvertUpgradeAttributeValueToString(const float InValue);
	FString GetUpgradeTextFromSingularAttributeValue(const TPair<EUpgradeAttribute, float> InAttribute);

	virtual void NativeConstruct() override;
	void ProcessCurrentUpgradeData(FUpgradeAttributes& PreviousUpgradeData, FUpgradeAttributes& CurrentUpgradeData);
	void SetUpgradeText();
	UFUNCTION()
	void SelectUpgrade();
	UFUNCTION()
	void PlayHoverAnim();
	UFUNCTION()
	void PlayUnHoverAnim();
};

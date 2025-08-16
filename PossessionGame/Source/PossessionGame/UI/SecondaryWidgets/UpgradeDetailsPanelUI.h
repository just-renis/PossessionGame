#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpgradeDetailsPanelUI.generated.h"

class UImage;
class UOverlay;
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
	TObjectPtr<UOverlay> UpgradeDetailsPanelOverlay;
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
	TObjectPtr<UUpgradeData> UpgradeData;
	UPROPERTY(EditDefaultsOnly, Category = "Textures", meta = (AllowPrivateAccess = true))
	TMap<int32, TObjectPtr<UTexture2D>> LevelsTextures;

	FString ConvertUpgradeAttributeValueToString(const float InValue);
	FString GetUpgradeTextFromSingularAttributeValue(const TPair<EUpgradeAttribute, float> InAttribute);

	virtual void NativeConstruct() override;
	void ProcessCurrentUpgradeData(FUpgradeAttributes& PreviousUpgradeData, FUpgradeAttributes& CurrentUpgradeData);
	void SetUpgradeText(const bool bIsPlayerUpgrade);
	UFUNCTION()
	void SelectUpgrade();
	UFUNCTION()
	void PlayHoverAnim();
	UFUNCTION()
	void PlayUnHoverAnim();
};

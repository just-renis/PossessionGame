#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "UpgradeDetailsPanelUI.generated.h"

class URichTextBlock;
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
	TArray<TObjectPtr<UTexture2D>> NormalTextures;
	UPROPERTY(EditDefaultsOnly, Category = "Textures", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UTexture2D>> HoverTextures;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> PlayerUpgradeText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> EnemyUpgradeText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> UpgradeChoiceButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> UpgradeDetailsPanelOverlay;
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
	EUpgradeAttribute SpecialAttribute = EUpgradeAttribute::None;
	UPROPERTY()
	FUpgradeChoicePair UpgradeChoice;

	FString ConvertUpgradeAttributeValueToString(const float InValue);
	FText GetUpgradeTextFromSingularAttributeValue(const TPair<EUpgradeAttribute, float> InAttribute, const bool bForEnemy);
	FString GetRichTextTag(const bool bForEnemy) const;
	FText GetSpecialUpgradeText(const EUpgradeAttribute InUpgradeAttribute, const bool bForEnemy);

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

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpgradeCardUI.generated.h"

enum class EUpgrade : uint8;
class UButton;
class UUpgradeData;
class UBorder;
class UTextBlock;
class UImage;

UCLASS()
class POSSESSIONGAME_API UUpgradeCardUI : public UUserWidget
{
	GENERATED_BODY()
public:
	TObjectPtr<UButton> GetUpgradeButton() const { return UpgradeButton; }
	
	void SetupCard(const TObjectPtr<UTexture2D>& InCardBackgroundImage, const TObjectPtr<UUpgradeData>& InData, const TObjectPtr<UTexture2D>& InCharacterIcon);
	void ToggleCardVisibility(const bool bState);
	void SetUpgradeText(const FText& InText) const;
	void PlayBlinkAnim() { PlayAnimation(BlinkAnim); }
	void PlayHoverAnim() { PlayAnimation(HoverAnim); }
	void PlayUnHoverAnim() { PlayAnimation(UnHoverAnim); }
	void PlaySelectAnim()  { PlayAnimation(SelectAnim); }
	void StopHoverAnim() { StopAnimation(HoverAnim); }
	void StopUnHoverAnim() { StopAnimation(UnHoverAnim); }
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> CardBackgroundBorder;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> UpgradeButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CardIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UpgradeText;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> BlinkAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> UnHoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> SelectAnim;
	UPROPERTY()
	TObjectPtr<UUpgradeData> Data;

	virtual void NativeConstruct() override;
	void SetCardBackgroundBorderImage(const TObjectPtr<UTexture2D>& InImage) const;
	void SetCharacterIcon(const TObjectPtr<UTexture2D>& InImage) const;
	void SetCardIcon(const TObjectPtr<UTexture2D>& InImage) const;
	UFUNCTION()
	void SelectUpgrade();
};

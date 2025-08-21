#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PossessionGame/Utilities/Data Utilities/FRuntimeUpgradeData.h"
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
	
	void SetupCard(const FRuntimeUpgradeData& InData, const TObjectPtr<UTexture2D>& InCharacterIcon);
	void ToggleCardVisibility(const bool bState);
	void SetUpgradeText(const FText& InText) const;
private:
	UPROPERTY(EditDefaultsOnly, Category = "Textures", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UTexture2D>> LevelTextures;
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
	FRuntimeUpgradeData Data;

	virtual void NativeConstruct() override;
	void SetCardBackgroundBorderImage(const int32 InLevel) const;
	void SetCharacterIcon(const TObjectPtr<UTexture2D>& InImage) const;
	void SetCardIcon(const TObjectPtr<UTexture2D>& InImage) const;
	void PlayBlinkAnim() { PlayAnimation(BlinkAnim); }
	UFUNCTION()
	void PlaySelectAnim()  { PlayAnimation(SelectAnim); }
	UFUNCTION()
	void SelectUpgrade();
	UFUNCTION()
	void PlayHoverAnim();
	UFUNCTION()
	void PlayUnHoverAnim();
};

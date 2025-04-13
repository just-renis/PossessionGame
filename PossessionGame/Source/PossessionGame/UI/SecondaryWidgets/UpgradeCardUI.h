#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpgradeCardUI.generated.h"

enum class EUpgrade : uint8;
enum class EUpgradeAttribute : uint8;
struct FLevelUpgradeData;
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
	
	void SetupCard(const bool bPlayerCard, const TObjectPtr<UTexture2D>& InCardBackgroundImage, const TObjectPtr<UUpgradeData>& InData, const TObjectPtr<UTexture2D>& InCharacterIcon, const TObjectPtr<UTexture2D>& InPlayerIcon = nullptr);
	void ToggleCardVisibility(const bool bState);
	void SetUpgradeText(const FString& InText) const;
	void PlayBlinkAnim() { if (BlinkAnim) PlayAnimation(BlinkAnim); }
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> CardBackgroundBorder;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> UpgradeButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PlayerOrEnemyIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CardIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CharacterText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UpgradeText;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> BlinkAnim;
	UPROPERTY()
	TObjectPtr<UUpgradeData> Data;

	virtual void NativeConstruct() override;
	void SetCardBackgroundBorderImage(const TObjectPtr<UTexture2D>& InImage) const;
	void SetCharacterIcon(const TObjectPtr<UTexture2D>& InImage) const;
	void SetPlayerIcon(const TObjectPtr<UTexture2D>& InImage) const;
	void SetPlayerOrEnemyIconInvisible() const;
	void SetCardIcon(const TObjectPtr<UTexture2D>& InImage) const;
	void SetCharacterText(const FText& InText, const FColor InColor) const;
	UFUNCTION()
	void SelectUpgrade();
};

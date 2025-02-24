#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpgradeCardUI.generated.h"

class UBorder;
class UTextBlock;
class UImage;

UCLASS()
class POSSESSIONGAME_API UUpgradeCardUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetCardBackgroundBorderImage(const TObjectPtr<UTexture2D>& InImage) const;
	void SetPlayerOrEnemyIcon(const TObjectPtr<UTexture2D>& InImage) const;
	void SetCharacterIcon(const TObjectPtr<UTexture2D>& InImage) const;
	void SetCardIcon(const TObjectPtr<UTexture2D>& InImage) const;
	void SetCharacterText(const FText& InText) const;
	void SetUpgradeText(const FText& InText) const;
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> CardBackgroundBorder;
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
};

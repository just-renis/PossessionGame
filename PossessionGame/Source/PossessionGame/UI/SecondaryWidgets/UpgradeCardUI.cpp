#include "UpgradeCardUI.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UUpgradeCardUI::SetCardBackgroundBorderImage(const TObjectPtr<UTexture2D>& InImage) const { CardBackgroundBorder->SetBrushFromTexture(InImage); }

void UUpgradeCardUI::SetPlayerOrEnemyIcon(const TObjectPtr<UTexture2D>& InImage) const { PlayerOrEnemyIcon->SetBrushFromTexture(InImage); }

void UUpgradeCardUI::SetCharacterIcon(const TObjectPtr<UTexture2D>& InImage) const { CharacterIcon->SetBrushFromTexture(InImage); }

void UUpgradeCardUI::SetCardIcon(const TObjectPtr<UTexture2D>& InImage) const { CardIcon->SetBrushFromTexture(InImage); }

void UUpgradeCardUI::SetCharacterText(const FText& InText) const { CharacterText->SetText(InText); }

void UUpgradeCardUI::SetUpgradeText(const FText& InText) const { UpgradeText->SetText(InText); }

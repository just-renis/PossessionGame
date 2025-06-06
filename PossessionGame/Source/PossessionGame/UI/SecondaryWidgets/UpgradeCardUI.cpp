#include "UpgradeCardUI.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UUpgradeCardUI::SetupCard(const bool bPlayerCard, const TObjectPtr<UTexture2D>& InCardBackgroundImage, const TObjectPtr<UUpgradeData>& InData, const TObjectPtr<UTexture2D>& InCharacterIcon, const TObjectPtr<UTexture2D>& InPlayerIcon)
{
	Data = InData;
	ToggleCardVisibility(true);
	SetCardBackgroundBorderImage(InCardBackgroundImage);
	SetCharacterIcon(InCharacterIcon);
	SetPlayerIcon(InPlayerIcon);
	SetCardIcon(bPlayerCard ? Data->GetPlayerCardIcon() : Data->GetEnemyCardIcon());
	if (bPlayerCard) SetCharacterText(FText::FromString(UEnum::GetValueAsString(Data->GetCharacter()).Replace(TEXT("ECharacterType::"), TEXT("")).ToUpper()), Data->GetCharacter() == ECharacterType::Ranger ? FColor::Red : FColor::Cyan);
	else
	{
		SetCharacterText(FText::FromString("ENEMY"), FColor::Red);
		SetUpgradeText(FText::FromString(" +10 Hp +1 Damage"));
		SetPlayerOrEnemyIconInvisible();
	}
}

void UUpgradeCardUI::ToggleCardVisibility(const bool bState)
{
	SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	if (bState && UpgradeButton) UpgradeButton->SetIsEnabled(bState);
}

void UUpgradeCardUI::SetUpgradeText(const FText& InText) const
{
	if (!UpgradeText) return;
	UpgradeText->SetText(InText);
}

void UUpgradeCardUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (UpgradeButton) UpgradeButton->OnClicked.AddDynamic(this, &UUpgradeCardUI::SelectUpgrade);
}

void UUpgradeCardUI::SetCardBackgroundBorderImage(const TObjectPtr<UTexture2D>& InImage) const { if (CardBackgroundBorder && InImage) CardBackgroundBorder->SetBrushFromTexture(InImage); }

void UUpgradeCardUI::SetCharacterIcon(const TObjectPtr<UTexture2D>& InImage) const { if (CharacterIcon && InImage) CharacterIcon->SetBrushFromTexture(InImage); }

void UUpgradeCardUI::SetPlayerIcon(const TObjectPtr<UTexture2D>& InImage) const { if (PlayerOrEnemyIcon && InImage) PlayerOrEnemyIcon->SetBrushFromTexture(InImage); }

void UUpgradeCardUI::SetPlayerOrEnemyIconInvisible() const { if (PlayerOrEnemyIcon) PlayerOrEnemyIcon->SetVisibility(ESlateVisibility::Collapsed); }

void UUpgradeCardUI::SetCardIcon(const TObjectPtr<UTexture2D>& InImage) const { if (CardIcon && InImage) CardIcon->SetBrushFromTexture(InImage); }

void UUpgradeCardUI::SetCharacterText(const FText& InText, const FColor InColor) const
{
	if (!CharacterText) return;
	CharacterText->SetText(InText);
	CharacterText->SetColorAndOpacity(InColor);
}

void UUpgradeCardUI::SelectUpgrade()
{
	if (!UpgradeButton) return;
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ClickArrow);
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnUpgradeCardSelected.ExecuteIfBound(Data);
}

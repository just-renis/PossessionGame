#include "UpgradeCardUI.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UUpgradeCardUI::SetupCard(const TObjectPtr<UTexture2D>& InCardBackgroundImage, const TObjectPtr<UUpgradeData>& InData, const TObjectPtr<UTexture2D>& InCharacterIcon)
{
	Data = InData;
	ToggleCardVisibility(true);
	SetCardBackgroundBorderImage(InCardBackgroundImage);
	SetCharacterIcon(InCharacterIcon);
	SetCardIcon(Data->GetPlayerCardIcon());
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
	if (!UpgradeButton) return;
	UpgradeButton->OnClicked.AddDynamic(this, &UUpgradeCardUI::SelectUpgrade);
	UpgradeButton->OnHovered.AddDynamic(this, &UUpgradeCardUI::PlayHoverAnim);
	UpgradeButton->OnUnhovered.AddDynamic(this, &UUpgradeCardUI::PlayUnHoverAnim);
}

void UUpgradeCardUI::SetCardBackgroundBorderImage(const TObjectPtr<UTexture2D>& InImage) const { if (CardBackgroundBorder && InImage) CardBackgroundBorder->SetBrushFromTexture(InImage); }

void UUpgradeCardUI::SetCharacterIcon(const TObjectPtr<UTexture2D>& InImage) const { if (CharacterIcon && InImage) CharacterIcon->SetBrushFromTexture(InImage); }

void UUpgradeCardUI::SetCardIcon(const TObjectPtr<UTexture2D>& InImage) const { if (CardIcon && InImage) CardIcon->SetBrushFromTexture(InImage); }

void UUpgradeCardUI::SelectUpgrade()
{
	if (!UpgradeButton) return;
	PlaySelectAnim();
	PlayBlinkAnim();
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ClickArrow);
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnUpgradeCardSelected.ExecuteIfBound(Data);
}

void UUpgradeCardUI::PlayHoverAnim()
{
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ButtonHover);
	StopAnimation(UnHoverAnim);
	PlayAnimation(HoverAnim);
}

void UUpgradeCardUI::PlayUnHoverAnim()
{
	StopAnimation(HoverAnim);
	PlayAnimation(UnHoverAnim);
}

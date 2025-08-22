#include "UpgradeCardUI.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Data Utilities/FRuntimeUpgradeData.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UUpgradeCardUI::SetupCard(const FRuntimeUpgradeData& InData, const TObjectPtr<UTexture2D>& InCharacterIcon)
{
	Data = InData;
	ToggleCardVisibility(true);
	SetCharacterIcon(InCharacterIcon);
	SetCardIcon(Data.GetPlayerCardIcon());
}

void UUpgradeCardUI::ToggleCardVisibility(const bool bState)
{
	SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
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

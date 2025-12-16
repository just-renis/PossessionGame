#include "SecretBoostCardUI.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Enums/EVariation.h"
#include "PossessionGame/UI/MainWidgets/SecretRoomUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void USecretBoostCardUI::DisableButton() const { if (BoostButton) BoostButton->SetIsEnabled(false); }

void USecretBoostCardUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (BoostTexture && CardIcon) CardIcon->SetBrushFromTexture(BoostTexture);
	if (SecretBoostText) SecretBoostText->SetText(BoostText);
}

void USecretBoostCardUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (!BoostButton) return;
	BoostButton->OnHovered.AddDynamic(this, &USecretBoostCardUI::OnHovered);
	BoostButton->OnUnhovered.AddDynamic(this, &USecretBoostCardUI::OnUnHovered);
	BoostButton->OnClicked.AddDynamic(this, &USecretBoostCardUI::SaveChosenBoost);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnSecretBoostSelected.AddUObject(this, &USecretBoostCardUI::PlayNotSelectedAnim);
}

void USecretBoostCardUI::NativeDestruct()
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnSecretBoostSelected.RemoveAll(this);
	Super::NativeDestruct();
}

void USecretBoostCardUI::PlayNotSelectedAnim() { if (!bChosen) PlayAnimationForward(NotSelectAnim); }

void USecretBoostCardUI::OnHovered()
{
	StopAnimation(UnHoverAnim);
	PlayAnimation(HoverAnim);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ButtonHover);
	switch (BoostType)
	{
		case EVariation::SecretCrateRewardHealth: (void)ShowDetailsPanelContents.ExecuteIfBound(true, FText::Format(NSLOCTEXT("ST_Localization", "SecretHpBoost", "Grants {0}% Increased Max HP This Run"), FText::AsNumber(BoostValue))); return;
		case EVariation::SecretCrateRewardDamage: (void)ShowDetailsPanelContents.ExecuteIfBound(true,  FText::Format(NSLOCTEXT("ST_Localization", "SecretDamageBoost",  "Grants {0}% Increased Max Damage This Run"), FText::AsNumber(BoostValue))); return;
		case EVariation::SecretCrateRewardXp: (void)ShowDetailsPanelContents.ExecuteIfBound(true,  FText::Format(NSLOCTEXT("ST_Localization", "SecretXPBoost",  "Grants {0} Level and Upgrade XP"), FText::AsNumber(BoostValue)));
		default: ;
	}
}

void USecretBoostCardUI::OnUnHovered()
{
	StopAnimation(HoverAnim);
	PlayAnimation(UnHoverAnim);
	(void)ShowDetailsPanelContents.ExecuteIfBound(false, FText::FromString(""));
}

void USecretBoostCardUI::SaveChosenBoost()
{
	PlayAnimation(SelectAnim);
	bChosen = true;
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::UpgradeCardSelect);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnSecretBoostSelected.Broadcast();
	if (BoostType == EVariation::SecretCrateRewardXp)
	{
		(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnXpReceived.ExecuteIfBound(BoostValue);
	}
	else
	{
		GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->AddSecretCrateBoost(BoostType, BoostValue);
		if (BoostType == EVariation::SecretCrateRewardHealth) (void)GetGameInstance()->GetSubsystem<UDelegateManager>()->ApplySecretBoostHealth.ExecuteIfBound(BoostValue);
	}
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<USecretRoomUI>(EWidget::SecretRoomUI)->CloseUI();
}

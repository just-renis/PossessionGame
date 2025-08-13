#include "AbilityIconUI.h"
#include "Components/Button.h"
#include "Components/ButtonSlot.h"
#include "Components/Image.h"
#include "PossessionGame/UI/MainWidgets/AbilitiesMenuUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UAbilityIconUI::SetIsInSlot(const bool bState)
{
	bIsInSlot = bState;
	if (AbilityButton) AbilityButton->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible :ESlateVisibility::Visible);
}

void UAbilityIconUI::SetData(const ECharacterType InCharacter, const FAbilityIconData& InData)
{
	DetailsText = InData.Details;
	AbilityIndex = InData.AbilityIndex;
	if (AbilityIndex == 0 || GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetIsAbilityUnlocked(InCharacter, AbilityIndex)) bIsLocked = false;
	if (!AbilityIcon) return;
	if (const TObjectPtr<UButtonSlot> ButtonSlot = Cast<UButtonSlot>(AbilityIcon->Slot)) ButtonSlot->SetPadding(bIsLocked ? FMargin(0.f) : FMargin(15.f));
	AbilityIcon->SetBrushFromAtlasInterface(bIsLocked ? LockedTexture : InData.AbilityIcon);
}

void UAbilityIconUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (!AbilityButton) return;
	AbilityButton->OnHovered.AddDynamic(this, &UAbilityIconUI::OnHovered);
	AbilityButton->OnUnhovered.AddDynamic(this, &UAbilityIconUI::OnUnHovered);
}

void UAbilityIconUI::OnHovered()
{
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ButtonHover);
	PlayAnimationForward(HoverAnim);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UAbilitiesMenuUI>(EWidget::AbilitiesMenuUI)->ToggleDetailsPanel(true, DetailsText);
}

void UAbilityIconUI::OnUnHovered()
{
	PlayAnimationReverse(HoverAnim);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UAbilitiesMenuUI>(EWidget::AbilitiesMenuUI)->ToggleDetailsPanel(false, DetailsText);
}

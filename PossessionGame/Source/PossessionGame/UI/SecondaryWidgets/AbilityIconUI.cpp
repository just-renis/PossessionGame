#include "AbilityIconUI.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "PossessionGame/UI/MainWidgets/AbilitiesMenuUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
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
	if (AbilityIndex == 0 || Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot()->GetIsAbilityUnlocked(InCharacter, AbilityIndex)) bIsLocked = false;
	if (AbilityIcon) AbilityIcon->SetBrushFromAtlasInterface(bIsLocked ? LockedTexture : InData.AbilityIcon);
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
	PlayAnimationForward(HoverAnim);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UAbilitiesMenuUI>(EWidget::AbilitiesMenuUI, 1)->ToggleDetailsPanel(true, DetailsText);
}

void UAbilityIconUI::OnUnHovered()
{
	PlayAnimationReverse(HoverAnim);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UAbilitiesMenuUI>(EWidget::AbilitiesMenuUI, 1)->ToggleDetailsPanel(false, DetailsText);
}

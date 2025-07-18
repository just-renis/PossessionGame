#include "AbilityIconUI.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "PossessionGame/UI/MainWidgets/AbilitiesMenuUI.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UAbilityIconUI::SetAbilityData(const TObjectPtr<UTexture2D>& InIcon, const FText& InDetailsText)
{
	if (AbilityIcon) AbilityIcon->SetBrushFromAtlasInterface(InIcon);
	DetailsText = InDetailsText;
}

void UAbilityIconUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (!AbilityButton) return;
	AbilityButton->OnClicked.AddDynamic(this, &UAbilityIconUI::ProcessClickEvent);
	AbilityButton->OnHovered.AddDynamic(this, &UAbilityIconUI::OnHovered);
	AbilityButton->OnUnhovered.AddDynamic(this, &UAbilityIconUI::OnUnHovered);
}

void UAbilityIconUI::ProcessClickEvent()
{
	
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

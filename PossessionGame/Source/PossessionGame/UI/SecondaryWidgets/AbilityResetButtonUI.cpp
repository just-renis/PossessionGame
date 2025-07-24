#include "AbilityResetButtonUI.h"
#include "Components/Button.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UAbilityResetButtonUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (!AbilityResetButton) return;
	AbilityResetButton->OnPressed.AddDynamic(this, &UAbilityResetButtonUI::OnSelect);
	AbilityResetButton->OnHovered.AddDynamic(this, &UAbilityResetButtonUI::OnHovered);
	AbilityResetButton->OnUnhovered.AddDynamic(this, &UAbilityResetButtonUI::OnUnHovered);
}

void UAbilityResetButtonUI::OnHovered()
{
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ButtonHover);
	PlayAnimationForward(HoverAnim);
}

void UAbilityResetButtonUI::OnUnHovered() { PlayAnimationReverse(HoverAnim); }

void UAbilityResetButtonUI::OnSelect()
{
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::AbilityReset);
	PlayAnimationForward(SelectAnim);
}

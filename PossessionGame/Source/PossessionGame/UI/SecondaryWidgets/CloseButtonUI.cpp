#include "CloseButtonUI.h"
#include "Components/Button.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UCloseButtonUI::NativeConstruct()
{
	Super::NativeConstruct();
	Button->OnClicked.AddDynamic(this, &UCloseButtonUI::OnClicked);
	Button->OnHovered.AddDynamic(this, &UCloseButtonUI::OnHovered);
	Button->OnUnhovered.AddDynamic(this, &UCloseButtonUI::OnUnHovered);
}

void UCloseButtonUI::OnHovered() { PlayAnimation(HoverAnim); }

void UCloseButtonUI::OnUnHovered()
{
	PlayAnimation(HoverAnim, GetAnimationCurrentTime(HoverAnim), 1, EUMGSequencePlayMode::Reverse);
}

void UCloseButtonUI::OnClicked()
{
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ClickArrow);
	PlayAnimation(ClickAnim);
}

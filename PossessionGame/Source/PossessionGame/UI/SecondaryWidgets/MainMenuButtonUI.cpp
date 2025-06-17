#include "MainMenuButtonUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UMainMenuButtonUI::SetText(const FText& InText) const { if (ButtonText) ButtonText->SetText(InText); }

void UMainMenuButtonUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (!ButtonText) return;
	ButtonText->SetText(Text);
	FSlateFontInfo FontInfo = ButtonText->GetFont();
	FontInfo.Size = FontSize;
	ButtonText->SetFont(FontInfo);
}

void UMainMenuButtonUI::NativeConstruct()
{
	Super::NativeConstruct();
	SetupButtonEvents();
}

void UMainMenuButtonUI::SetupButtonEvents()
{
	if (!Button) return;
	Button->OnClicked.AddDynamic(this, &UMainMenuButtonUI::TriggerClickAnim);
	Button->OnHovered.AddDynamic(this, &UMainMenuButtonUI::TriggerHoverAnim);
	Button->OnUnhovered.AddDynamic(this, &UMainMenuButtonUI::TriggerUnHoverAnim);
}

void UMainMenuButtonUI::TriggerClickAnim()
{
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ButtonSelect);
	PlayAnimation(ButtonClickAnim);
}

void UMainMenuButtonUI::TriggerHoverAnim()
{
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ButtonHover);
	PlayAnimation(ButtonHoverAnim, GetAnimationCurrentTime(ButtonHoverAnim));
}

void UMainMenuButtonUI::TriggerUnHoverAnim()
{
	PlayAnimation(ButtonHoverAnim, GetAnimationCurrentTime(ButtonHoverAnim), 1, EUMGSequencePlayMode::Reverse);
}
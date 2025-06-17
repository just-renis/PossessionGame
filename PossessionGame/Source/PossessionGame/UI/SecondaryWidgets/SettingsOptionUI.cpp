#include "SettingsOptionUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void USettingsOptionUI::SetText(const FText& InText) const { if (Text) Text->SetText(InText); }

void USettingsOptionUI::NativeConstruct()
{
	Super::NativeConstruct();
	SetupButtonEvents();
}

void USettingsOptionUI::OnLeftArrowClicked()
{
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ClickArrow);
	PlayAnimation(LeftArrowClickAnim);
}

void USettingsOptionUI::OnRightArrowClicked()
{
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ClickArrow);
	PlayAnimation(RightArrowClickAnim);
}

void USettingsOptionUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (Text)
	{
		FSlateFontInfo FontInfo = Text->GetFont();
		FontInfo.Size = FontSize;
		Text->SetFont(FontInfo);
	}
}

void USettingsOptionUI::SetupButtonEvents()
{
	if (UpscaleButton)
	{
		UpscaleButton->OnClicked.AddDynamic(this, &USettingsOptionUI::OnRightArrowClicked);
		UpscaleButton->OnHovered.AddDynamic(this, &USettingsOptionUI::OnRightArrowHovered);
		UpscaleButton->OnUnhovered.AddDynamic(this, &USettingsOptionUI::OnRightArrowUnHovered);
	}
	if (DownscaleButton)
	{
		DownscaleButton->OnClicked.AddDynamic(this, &USettingsOptionUI::OnLeftArrowClicked);
		DownscaleButton->OnHovered.AddDynamic(this, &USettingsOptionUI::OnLeftArrowHovered);
		DownscaleButton->OnUnhovered.AddDynamic(this, &USettingsOptionUI::OnLeftArrowUnHovered);
	}
}

void USettingsOptionUI::OnLeftArrowHovered()
{
	PlayAnimation(LeftArrowHoverAnim, GetAnimationCurrentTime(LeftArrowHoverAnim), 1, EUMGSequencePlayMode::Forward);
}

void USettingsOptionUI::OnLeftArrowUnHovered()
{
	PlayAnimation(LeftArrowHoverAnim, GetAnimationCurrentTime(LeftArrowHoverAnim), 1, EUMGSequencePlayMode::Reverse);
}

void USettingsOptionUI::OnRightArrowHovered()
{
	PlayAnimation(RightArrowHoverAnim, GetAnimationCurrentTime(RightArrowHoverAnim), 1, EUMGSequencePlayMode::Forward);
}

void USettingsOptionUI::OnRightArrowUnHovered()
{
	PlayAnimation(RightArrowHoverAnim, GetAnimationCurrentTime(RightArrowHoverAnim), 1, EUMGSequencePlayMode::Reverse);
}

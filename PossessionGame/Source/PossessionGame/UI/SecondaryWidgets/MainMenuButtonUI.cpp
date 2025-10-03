#include "MainMenuButtonUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UMainMenuButtonUI::SetText(const FText& InText) const { if (ButtonText) ButtonText->SetText(InText); }

void UMainMenuButtonUI::SetButtonVisibility(const bool bState)
{
	if (!Button) return;
	SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
	Button->SetVisibility(bState ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	SetIsEnabled(bState);
}

void UMainMenuButtonUI::SetFontSize(const int32 InFontSize) const
{
	FSlateFontInfo FontInfo = ButtonText->GetFont();
	FontInfo.Size = InFontSize;
	ButtonText->SetFont(FontInfo);
}

void UMainMenuButtonUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (!ButtonText) return;
	ButtonText->SetText(Text);
	FSlateFontInfo FontInfo = ButtonText->GetFont();
	FontInfo.Size = FontSize;
	ButtonText->SetFont(FontInfo);
	if (!BackgroundTexture) return;
	FButtonStyle Style = Button->GetStyle();
	Style.Normal.SetResourceObject(BackgroundTexture);
	Style.Hovered.SetResourceObject(BackgroundTexture);
	Style.Pressed.SetResourceObject(BackgroundTexture);
	Button->SetStyle(Style);
}

void UMainMenuButtonUI::NativeConstruct()
{
	Super::NativeConstruct();
	SetupDynamicMaterialForButton();
	SetupButtonEvents();
}

void UMainMenuButtonUI::SetupButtonEvents()
{
	if (!Button) return;
	Button->OnClicked.AddDynamic(this, &UMainMenuButtonUI::TriggerClickAnim);
	Button->OnHovered.AddDynamic(this, &UMainMenuButtonUI::TriggerHoverAnim);
	Button->OnUnhovered.AddDynamic(this, &UMainMenuButtonUI::TriggerUnHoverAnim);
}

void UMainMenuButtonUI::SetupDynamicMaterialForButton()
{
	const FButtonStyle& Style = Button->GetStyle();
	const TObjectPtr<UObject> Resource = Style.Normal.GetResourceObject();
	if (const TObjectPtr<UMaterialInterface> Material = Cast<UMaterialInterface>(Resource))
	{
		MID_ButtonBackground = UMaterialInstanceDynamic::Create(Material, this);
		FButtonStyle NewStyle = Style;
		NewStyle.Normal.SetResourceObject(MID_ButtonBackground);
		NewStyle.Hovered.SetResourceObject(MID_ButtonBackground);
		NewStyle.Pressed.SetResourceObject(MID_ButtonBackground);
		Button->SetStyle(NewStyle);
	}
}

void UMainMenuButtonUI::TriggerClickAnim()
{
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(bStrongButtonSelect ? ESound::ButtonSelect : ESound::ClickArrow);
	PlayAnimation(ButtonClickAnim);
}

void UMainMenuButtonUI::TriggerHoverAnim()
{
	if (MID_ButtonBackground) MID_ButtonBackground->SetScalarParameterValue(FName("HoverValue"), 3.f);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ButtonHover);
	PlayAnimation(ButtonHoverAnim, GetAnimationCurrentTime(ButtonHoverAnim));
}

void UMainMenuButtonUI::TriggerUnHoverAnim()
{
	if (MID_ButtonBackground) MID_ButtonBackground->SetScalarParameterValue(FName("HoverValue"), 1.f);
	PlayAnimation(ButtonHoverAnim, GetAnimationCurrentTime(ButtonHoverAnim), 1, EUMGSequencePlayMode::Reverse);
}
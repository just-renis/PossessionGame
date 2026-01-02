#include "PlayerLevelSystemUI.h"
#include "PlayerHUD.h"
#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/ELevelSystemUpgradeTexture.h"
#include "PossessionGame/Enums/EPlayerLevelUpgradeId.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/UI/SecondaryWidgets/PlayerLevelOptionUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UPlayerLevelSystemUI::OpenUI() { Toggle(true); }

void UPlayerLevelSystemUI::CloseUI()
{
	Toggle(false);
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnAbilitiesOrLevelMenuSelectionEnd.ExecuteIfBound(EWidget::PlayerLevelSystemUI);
}

TObjectPtr<UWidget> UPlayerLevelSystemUI::GenerateLevelText(const int32 InIndex) const
{
	const TObjectPtr<UTextBlock> LevelText = WidgetTree->ConstructWidget<UTextBlock>();
	LevelText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "Level", "Level {0}"), InIndex));
	FSlateFontInfo FontInfo = LevelText->GetFont();
	FontInfo.FontObject = Font;
	LevelText->SetFont(FontInfo);
	return LevelText;
}

void UPlayerLevelSystemUI::NativeConstruct()
{
	Super::NativeConstruct();
	FadeOutDelegate.BindDynamic(this, &UPlayerLevelSystemUI::Hide);
	SetupPlayerOptions();
}

void UPlayerLevelSystemUI::SetupPlayerOptions()
{
	if (!OptionsBox) return;
	const TObjectPtr<USaveSlot> SaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot();
	for (int32 i = 0; i < PlayerLevelMaxAmount; i++)
	{
		const TObjectPtr<UHorizontalBox> HorizontalBox = WidgetTree->ConstructWidget<UHorizontalBox>();
		if (!HorizontalBox) continue;
		if (const TObjectPtr<UVerticalBoxSlot> HorizontalBoxSlot = Cast<UVerticalBoxSlot>(OptionsBox->AddChild(HorizontalBox)))
		{
			HorizontalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
			if (const TObjectPtr<UHorizontalBoxSlot> LevelTextElement = Cast<UHorizontalBoxSlot>(HorizontalBox->AddChild(GenerateLevelText(i + 1)))) SetupLevelText(LevelTextElement);
			SetupLevelOptions(i, SaveSlot, HorizontalBox);
		}
		SetupSpacerBetweenLevelOptions();
	}
}

void UPlayerLevelSystemUI::Toggle(const bool bState)
{
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPlayerHUD>(EWidget::PlayerHUD)->PlayFadeOutAnim(bState);
	UGameplayStatics::SetGamePaused(GetWorld(), bState);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(bState ? ESound::PanelFadeIn : ESound::PanelFadeOut);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	GetWorld()->GetSubsystem<UWidgetManager>()->SetCurrentStateOfUI(bState ? EWidget::PlayerLevelSystemUI : EWidget::None);
	if (bState)
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		PlayerController->SetInputMode(FInputModeGameAndUI());
	}
	else
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		BindToAnimationFinished(FadeInAnim, FadeOutDelegate);
	}
	PlayAnimation(FadeInAnim, GetAnimationCurrentTime(FadeInAnim), 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
}

void UPlayerLevelSystemUI::SetupLevelText(const TObjectPtr<UHorizontalBoxSlot>& InElement)
{
	FSlateChildSize Size(ESlateSizeRule::Fill);
	Size.Value = 0.4f;
	InElement->SetSize(Size);
	InElement->SetPadding(FMargin(5.f, 0.f, 0.f, 0.f));
	InElement->SetHorizontalAlignment(HAlign_Fill);
	InElement->SetVerticalAlignment(VAlign_Center);
}

void UPlayerLevelSystemUI::SetupFirstLevelOption(const TObjectPtr<UHorizontalBoxSlot>& InElement)
{
	FSlateChildSize Size(ESlateSizeRule::Fill);
	Size.Value = 1.f;
	InElement->SetSize(Size);
	InElement->SetPadding(FMargin(0.f, 0.f, 25.f, 0.f));
}

void UPlayerLevelSystemUI::SetupSecondLevelOption(const TObjectPtr<UHorizontalBoxSlot>& InElement)
{
	FSlateChildSize Size(ESlateSizeRule::Fill);
	Size.Value = 1.f;
	InElement->SetSize(Size);
}

void UPlayerLevelSystemUI::SetupLevelOptions(const int32 InIndex, const TObjectPtr<USaveSlot>& InSaveSlot, const TObjectPtr<UHorizontalBox>& InHorizontalBox)
{
	const TObjectPtr<UPlayerLevelOptionUI> FirstPlayerLevelOptionWidget = CreateWidget<UPlayerLevelOptionUI>(this, PlayerLevelOptionClass);
	if (const TObjectPtr<UHorizontalBoxSlot> FirstLevelOption = Cast<UHorizontalBoxSlot>(InHorizontalBox->AddChild(FirstPlayerLevelOptionWidget))) SetupFirstLevelOption(FirstLevelOption);
	const TObjectPtr<UPlayerLevelOptionUI> SecondPlayerLevelOptionWidget = CreateWidget<UPlayerLevelOptionUI>(this, PlayerLevelOptionClass);
	if (const TObjectPtr<UHorizontalBoxSlot> SecondLevelOption = Cast<UHorizontalBoxSlot>(InHorizontalBox->AddChild(SecondPlayerLevelOptionWidget))) SetupSecondLevelOption(SecondLevelOption);
	const EPlayerLevelUpgradeId FirstId = static_cast<EPlayerLevelUpgradeId>(InIndex * 2);
	const EPlayerLevelUpgradeId SecondId = static_cast<EPlayerLevelUpgradeId>(InIndex * 2 + 1);
	TObjectPtr<UObject> TextureForLevelOptionIcon = IconTextures[ELevelSystemUpgradeTexture::Ability];
	if (const FText* Description = UpgradeDescriptions.Find(FirstId))
	{
		if (const TObjectPtr<UObject>* Texture = IconTextures.Find(LevelOptionsTextureTypes[InIndex * 2])) TextureForLevelOptionIcon = *Texture;
		FirstPlayerLevelOptionWidget->Setup(InSaveSlot->GetPlayerLevel() > InIndex, SecondPlayerLevelOptionWidget, FirstId, SecondId, *Description, InSaveSlot->IsPlayerLevelUpgradeSelected(FirstId), TextureForLevelOptionIcon);
	}
	if (const FText* Description = UpgradeDescriptions.Find(SecondId))
	{
		if (const TObjectPtr<UObject>* Texture = IconTextures.Find(LevelOptionsTextureTypes[InIndex * 2 + 1])) TextureForLevelOptionIcon = *Texture;
		SecondPlayerLevelOptionWidget->Setup(InSaveSlot->GetPlayerLevel() > InIndex, FirstPlayerLevelOptionWidget, SecondId, FirstId, *Description, InSaveSlot->IsPlayerLevelUpgradeSelected(SecondId), TextureForLevelOptionIcon);
	}
}

void UPlayerLevelSystemUI::SetupSpacerBetweenLevelOptions() const
{
	const TObjectPtr<USpacer> Spacer = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass());
	if (const TObjectPtr<UVerticalBoxSlot> ElementSlot = Cast<UVerticalBoxSlot>(OptionsBox->AddChild(Spacer)))
	{
		FSlateChildSize Size(ESlateSizeRule::Fill);
		Size.Value = 0.1f;
		ElementSlot->SetSize(Size);
	}
}

void UPlayerLevelSystemUI::Hide()
{
	UnbindAllFromAnimationFinished(FadeInAnim);
	SetVisibility(ESlateVisibility::Collapsed);
}

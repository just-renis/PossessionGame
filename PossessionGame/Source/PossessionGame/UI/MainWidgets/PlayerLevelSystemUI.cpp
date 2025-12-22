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
#include "PossessionGame/Enums/EPlayerLevelUpgradeId.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/UI/SecondaryWidgets/PlayerLevelOptionUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UPlayerLevelSystemUI::OpenUI() { Toggle(true); }

void UPlayerLevelSystemUI::CloseUI() { Toggle(false); }

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
			FSlateChildSize HorizontalBoxSize(ESlateSizeRule::Fill);
			HorizontalBoxSlot->SetSize(HorizontalBoxSize);
			const TObjectPtr<UTextBlock> LevelText = WidgetTree->ConstructWidget<UTextBlock>();
			if (!LevelText) continue;
			LevelText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "Level", "Level {0}"), i + 1));
			if (const TObjectPtr<UHorizontalBoxSlot> ElementSlot = Cast<UHorizontalBoxSlot>(HorizontalBox->AddChild(LevelText)))
			{
				FSlateChildSize Size(ESlateSizeRule::Fill);
				Size.Value = 0.4f;
				ElementSlot->SetSize(Size);
				ElementSlot->SetPadding(FMargin(5.f, 0.f, 0.f, 0.f));
				ElementSlot->SetHorizontalAlignment(HAlign_Fill);
				ElementSlot->SetVerticalAlignment(VAlign_Center);
			}
			const TObjectPtr<UPlayerLevelOptionUI> FirstPlayerLevelOptionWidget = CreateWidget<UPlayerLevelOptionUI>(this, PlayerLevelOptionClass);
			if (!FirstPlayerLevelOptionWidget) continue;
			if (const TObjectPtr<UHorizontalBoxSlot> ElementSlot = Cast<UHorizontalBoxSlot>(HorizontalBox->AddChild(FirstPlayerLevelOptionWidget)))
			{
				FSlateChildSize Size(ESlateSizeRule::Fill);
				Size.Value = 1.f;
				ElementSlot->SetSize(Size);
				ElementSlot->SetPadding(FMargin(0.f, 0.f, 25.f, 0.f));
			}
			const TObjectPtr<UPlayerLevelOptionUI> SecondPlayerLevelOptionWidget = CreateWidget<UPlayerLevelOptionUI>(this, PlayerLevelOptionClass);
			if (!SecondPlayerLevelOptionWidget) continue;
			if (const TObjectPtr<UHorizontalBoxSlot> ElementSlot = Cast<UHorizontalBoxSlot>(HorizontalBox->AddChild(SecondPlayerLevelOptionWidget)))
			{
				FSlateChildSize Size(ESlateSizeRule::Fill);
				Size.Value = 1.f;
				ElementSlot->SetSize(Size);
			}
			const EPlayerLevelUpgradeId FirstId = static_cast<EPlayerLevelUpgradeId>(i * 2);
			const EPlayerLevelUpgradeId SecondId = static_cast<EPlayerLevelUpgradeId>(i * 2 + 1);
			if (const FText* Description = UpgradeDescriptions.Find(FirstId)) FirstPlayerLevelOptionWidget->Setup(SecondPlayerLevelOptionWidget, FirstId, SecondId, *Description, SaveSlot->IsPlayerLevelUpgradeSelected(FirstId));
			if (const FText* Description = UpgradeDescriptions.Find(SecondId)) SecondPlayerLevelOptionWidget->Setup(FirstPlayerLevelOptionWidget, SecondId, FirstId, *Description, SaveSlot->IsPlayerLevelUpgradeSelected(SecondId));
		}
		const TObjectPtr<USpacer> Spacer = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass());
		if (!Spacer) continue;
		if (const TObjectPtr<UVerticalBoxSlot> ElementSlot = Cast<UVerticalBoxSlot>(OptionsBox->AddChild(Spacer)))
		{
			FSlateChildSize Size(ESlateSizeRule::Fill);
			Size.Value = 0.1f;
			ElementSlot->SetSize(Size);
		}
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

void UPlayerLevelSystemUI::Hide()
{
	UnbindAllFromAnimationFinished(FadeInAnim);
	SetVisibility(ESlateVisibility::Collapsed);
}

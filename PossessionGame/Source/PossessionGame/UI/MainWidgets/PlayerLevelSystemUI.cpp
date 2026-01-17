#include "PlayerLevelSystemUI.h"
#include "PlayerHUD.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
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
#include "PossessionGame/UI/SecondaryWidgets/CloseButtonUI.h"
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

FString UPlayerLevelSystemUI::GetRichTag(const bool bIsNumberTag) { return bIsNumberTag ? FString("<BigUpgradeAttributeGreen>") : FString("<BigUpgradeAttributeText>"); }

void UPlayerLevelSystemUI::NativeConstruct()
{
	Super::NativeConstruct();
	FadeOutDelegate.BindDynamic(this, &UPlayerLevelSystemUI::Hide);
	SetupPlayerOptions();
	CloseButton->Get()->OnClicked.AddDynamic(this, &UPlayerLevelSystemUI::CloseUI);
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
	SetupLevelOptionsDescriptions();
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

void UPlayerLevelSystemUI::SetupLevelOptionsDescriptions()
{
	UpgradeDescriptions.Add(EPlayerLevelUpgradeId::FirstOptionA, FText::Format(NSLOCTEXT("ST_Localization", "LevelUpgradeFirstOptionA", "{0}to all damage{1}"),
		FText::FromString(GetRichTag(true) + "+15% </>" + GetRichTag(false)), FText::FromString("</>")));
	UpgradeDescriptions.Add(EPlayerLevelUpgradeId::FirstOptionB, FText::Format(NSLOCTEXT("ST_Localization", "LevelUpgradeFirstOptionB", "{0}to all characters HP{1}"),
		FText::FromString(GetRichTag(true) + "+25% </>" + GetRichTag(false)), FText::FromString("</>")));
	UpgradeDescriptions.Add(EPlayerLevelUpgradeId::SecondOptionA, FText::Format(NSLOCTEXT("ST_Localization", "LevelUpgradeSecondOptionA", "{0}hijacking speed{1}"),
		FText::FromString(GetRichTag(true) + "+30% </>" + GetRichTag(false)), FText::FromString("</>")));
	UpgradeDescriptions.Add(EPlayerLevelUpgradeId::SecondOptionB, FText::Format(NSLOCTEXT("ST_Localization", "LevelUpgradeSecondOptionB", "{0}Hijacking a damaged robot restores{1}max HP{2}"),
		FText::FromString(GetRichTag(false)), FText::FromString("</>" + GetRichTag(true) + " 25% </>" + GetRichTag(false)), FText::FromString("</>")));
	UpgradeDescriptions.Add(EPlayerLevelUpgradeId::ThirdOptionA, FText::Format(NSLOCTEXT("ST_Localization", "LevelUpgradeThirdOptionA", "{0}critical damage{1}"),
		FText::FromString(GetRichTag(true) + "+20% </>" + GetRichTag(false)), FText::FromString("</>")));
	UpgradeDescriptions.Add(EPlayerLevelUpgradeId::ThirdOptionB, FText::Format(NSLOCTEXT("ST_Localization", "LevelUpgradeThirdOptionB", "{0}all characters HP{1}"),
		FText::FromString(GetRichTag(true) + "+50 </>" + GetRichTag(false)), FText::FromString("</>")));
	UpgradeDescriptions.Add(EPlayerLevelUpgradeId::FourthOptionA, FText::Format(NSLOCTEXT("ST_Localization", "LevelUpgradeFourthOptionA", "{0}Hijacking robot gives{1}all damage for{2}Boost does not stack{3}"),
		FText::FromString(GetRichTag(false)), FText::FromString("</>" + GetRichTag(true) + "+35% </>" + GetRichTag(false)), FText::FromString("</>" + GetRichTag(true) + " 6 s </>" + GetRichTag(false)), FText::FromString("</>")));
	UpgradeDescriptions.Add(EPlayerLevelUpgradeId::FourthOptionB, FText::Format(NSLOCTEXT("ST_Localization", "LevelUpgradeFourthOptionB", "{0}Hijacking robot gives{1}taken damage for{2}Boost does not stack{3}"),
		FText::FromString(GetRichTag(false)), FText::FromString("</>" + GetRichTag(true) + "-40% </>" + GetRichTag(false)), FText::FromString("</>" + GetRichTag(true) + " 4 s </>" + GetRichTag(false)), FText::FromString("</>")));
	UpgradeDescriptions.Add(EPlayerLevelUpgradeId::FifthOptionA, FText::Format(NSLOCTEXT("ST_Localization", "LevelUpgradeFifthOptionA", "{0}Quick melee and stomp for each player level gets{1}damage{2}"),
		FText::FromString(GetRichTag(false)), FText::FromString("</>" + GetRichTag(true) + " +15% </>" + GetRichTag(false)), FText::FromString("</>")));
	UpgradeDescriptions.Add(EPlayerLevelUpgradeId::FifthOptionB, FText::Format(NSLOCTEXT("ST_Localization", "LevelUpgradeFifthOptionB", "{0}All weapons for each player level gets{1}damage{2}"),
		FText::FromString(GetRichTag(false)), FText::FromString("</>" + GetRichTag(true) + " +5% </>" + GetRichTag(false)), FText::FromString("</>")));
	UpgradeDescriptions.Add(EPlayerLevelUpgradeId::SixthOptionA, FText::Format(NSLOCTEXT("ST_Localization", "LevelUpgradeSixthOptionA", "{0}all characters HP{1}"),
		FText::FromString(GetRichTag(true) + "+50% </>" + GetRichTag(false)), FText::FromString("</>")));
	UpgradeDescriptions.Add(EPlayerLevelUpgradeId::SixthOptionB, FText::Format(NSLOCTEXT("ST_Localization", "LevelUpgradeSixthOptionB", "{0}all damage{1}"),
		FText::FromString(GetRichTag(true) + "+30% </>" + GetRichTag(false)), FText::FromString("</>")));
	UpgradeDescriptions.Add(EPlayerLevelUpgradeId::SeventhOptionA, FText::Format(NSLOCTEXT("ST_Localization", "LevelUpgradeSeventhOptionA", "{0}Improve 'Ability cooldown reduction' boost after hijacking by{1}"),
    	FText::FromString(GetRichTag(false)), FText::FromString("</>" + GetRichTag(true) + " 100% -> 175%</>")));
	UpgradeDescriptions.Add(EPlayerLevelUpgradeId::SeventhOptionB, FText::Format(NSLOCTEXT("ST_Localization", "LevelUpgradeSeventhOptionB", "{0}Increase 'Ability cooldown reduction' boost duration after hijacking by{1}"),
		FText::FromString(GetRichTag(false)), FText::FromString("</>" + GetRichTag(true) + " 8 s -> 12 s</>")));
	UpgradeDescriptions.Add(EPlayerLevelUpgradeId::EighthOptionA, FText::Format(NSLOCTEXT("ST_Localization", "LevelUpgradeEighthOptionA", "{0}Weapons have{1}chance to deal double damage{2}"),
		FText::FromString(GetRichTag(false)), FText::FromString("</>" + GetRichTag(true) + " 25% </>" + GetRichTag(false)), FText::FromString("</>")));
	UpgradeDescriptions.Add(EPlayerLevelUpgradeId::EighthOptionB, FText::Format(NSLOCTEXT("ST_Localization", "LevelUpgradeEighthOptionB", "{0}critical damage{1}"),
		FText::FromString(GetRichTag(true) + "+40% </>" + GetRichTag(false)), FText::FromString("</>")));
	UpgradeDescriptions.Add(EPlayerLevelUpgradeId::NinthOptionA, FText::Format(NSLOCTEXT("ST_Localization", "LevelUpgradeNinthOptionA", "{0}Grants a stack which increases all damage by{1}for each ability used, stack duration{2}"),
		FText::FromString(GetRichTag(false)), FText::FromString("</>" + GetRichTag(true) + " 5% </>" + GetRichTag(false)), FText::FromString("</>" + GetRichTag(true) + "10 s</>")));
	UpgradeDescriptions.Add(EPlayerLevelUpgradeId::NinthOptionB, FText::Format(NSLOCTEXT("ST_Localization", "LevelUpgradeNinthOptionB", "{0}Grants a stack which boosts critical damage by{1}for each critical hit dealt, stack duration{2}"),
		FText::FromString(GetRichTag(false)), FText::FromString("</>" + GetRichTag(true) + " 10% </>" + GetRichTag(false)), FText::FromString("</>" + GetRichTag(true) + "8 s</>")));
	UpgradeDescriptions.Add(EPlayerLevelUpgradeId::TenthOptionA, FText::Format(NSLOCTEXT("ST_Localization", "LevelUpgradeTenthOptionA", "{0}All enemies give{1}more upgrade XP{2}"),
		FText::FromString(GetRichTag(false)), FText::FromString("</>" + GetRichTag(true) + " 35% </>" + GetRichTag(false)), FText::FromString("</>")));
	UpgradeDescriptions.Add(EPlayerLevelUpgradeId::TenthOptionB, FText::Format(NSLOCTEXT("ST_Localization", "LevelUpgradeTenthOptionB", "{0}Each kill within{1}of the previous kill increases the upgrade XP multiplier by{2}Bonus resets if the chain breaks{3}"),
		FText::FromString(GetRichTag(false)), FText::FromString("</>" + GetRichTag(true) + " 2 s </>" + GetRichTag(false)), FText::FromString("</>" + GetRichTag(true) + " 10%</>" + GetRichTag(false)), FText::FromString("</>")));
	
}

void UPlayerLevelSystemUI::Hide()
{
	UnbindAllFromAnimationFinished(FadeInAnim);
	SetVisibility(ESlateVisibility::Collapsed);
}

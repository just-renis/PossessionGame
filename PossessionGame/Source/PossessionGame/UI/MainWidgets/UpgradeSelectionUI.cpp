#include "UpgradeSelectionUI.h"
#include "PlayerHUD.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EExplanation.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/UI/SecondaryWidgets/MainMenuButtonUI.h"
#include "PossessionGame/UI/SecondaryWidgets/UpgradeCardUI.h"
#include "PossessionGame/UI/SecondaryWidgets/UpgradeDetailsPanelUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Utilities/Subsystems/UpgradeManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UUpgradeSelectionUI::OpenUI()
{
	BackButton->SetButtonVisibility(false);
	UpgradesData = GetWorld()->GetSubsystem<UUpgradeManager>()->GetRandomUpgrades();
	if (UpgradesData.IsEmpty())
	{
		CloseUI();
		(void)DelegateManager->OnUpgradeSelectionFinished.ExecuteIfBound();
		return;
	}
	PlayAnimation(UpgradeMenuOpenAnim);
	(void)DelegateManager->OnUpgradeSelectionOpened.ExecuteIfBound();
	Toggle(true);
	for (int32 i = 0; i < UpgradesData.Num(); i++)
	{
		CardOptions[i]->SetupCard(UpgradesData[i], GetCharacterUpgradeIcon(UpgradesData[i].GetCharacter()));
		CardOptions[i]->SetUpgradeText(GetUpgradeTextFromUpgrade(UpgradesData[i].GetUpgradeName()));
	}
	if (!GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetIsExplanationShown(EExplanation::UpgradeUI))
	{
		(void)GetWorld()->GetSubsystem<UWidgetManager>()->GetOnShowExplanationDelegate().ExecuteIfBound(EExplanation::UpgradeUI, GetGameInstance());
	}
}

void UUpgradeSelectionUI::CloseUI()
{
	for (int32 i = 0; i < UpgradesData.Num(); i++) CardOptions[i]->ToggleCardVisibility(false);
	UpgradeChoicesPanel->SetVisibility(ESlateVisibility::Collapsed);
	Toggle(false);
}

void UUpgradeSelectionUI::TriggerUpgradeMenuCloseAnimation()
{
	UpgradeChoice1->PlayDisappearAnim();
	UpgradeChoice2->PlayDisappearAnim();
	UpgradeChoice3->PlayDisappearAnim();
	PlayAnimation(UpgradeMenuCloseAnim);
	BindToAnimationFinished(UpgradeMenuCloseAnim, UpgradeSelectionFadeOutDelegate);
	PlayAnimByOptionSelected(false);
}

void UUpgradeSelectionUI::GoBack()
{
	if (!bChoicesOpen) return;
	PlayAnimByOptionSelected(true);
	bChoicesOpen = false;
	UpgradeChoice1->SetButtonVisibility(false);
	UpgradeChoice2->SetButtonVisibility(false);
	UpgradeChoice3->SetButtonVisibility(false);
	BackButton->SetButtonVisibility(false);
}

FText UUpgradeSelectionUI::GetUpgradeTextFromUpgrade(const EUpgrade InUpgrade)
{
	switch (InUpgrade)
	{
		case EUpgrade::RangerAbility1: return NSLOCTEXT("ST_Localization", "GrenadeLauncherUpgrade", "GRENADE LAUNCHER");
		case EUpgrade::RangerAbility2: return NSLOCTEXT("ST_Localization", "ShieldUpgrade", "SHIELD");
		case EUpgrade::TricksterAbility1: return NSLOCTEXT("ST_Localization", "PoisonUpgrade", "POISON");
		case EUpgrade::TricksterAbility2: return NSLOCTEXT("ST_Localization", "EmpUpgrade", "EMP");
		case EUpgrade::TricksterAbility3: return NSLOCTEXT("ST_Localization", "CloneUpgrade", "CLONE");
		case EUpgrade::GuardianAbility1: return NSLOCTEXT("ST_Localization", "ShieldUpgrade", "SHIELD");
		case EUpgrade::GuardianAbility2: return NSLOCTEXT("ST_Localization", "TimeStopUpgrade", "TIME STOP");
		default: ;
	}
	return FText::GetEmpty();
}

TObjectPtr<UTexture2D>& UUpgradeSelectionUI::GetCharacterUpgradeIcon(const ECharacterType InCharacter)
{
	switch (InCharacter)
	{
		case ECharacterType::Guardian: return GuardianIconTexture;
		case ECharacterType::Ranger: return RangerIconTexture;
		case ECharacterType::Trickster: return TricksterIconTexture;
		default: ;
	}
	return GuardianIconTexture;
}

void UUpgradeSelectionUI::Toggle(const bool bState)
{
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPlayerHUD>(EWidget::PlayerHUD)->PlayFadeOutAnim(bState);
	UGameplayStatics::SetGamePaused(GetWorld(), bState);
	SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(bState ? ESound::UpgradePanelFadeIn : ESound::UpgradePanelFadeOut);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	if (bState) PlayerController->SetInputMode(FInputModeGameAndUI());
	else PlayerController->SetInputMode(FInputModeGameOnly());
	GetWorld()->GetSubsystem<UWidgetManager>()->SetCurrentStateOfUI(bState ? EWidget::UpgradeSelectionUI : EWidget::None);
}

void UUpgradeSelectionUI::NativeConstruct()
{
	Super::NativeConstruct();
	DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	DelegateManager->OnUpgradeCardSelected.BindUObject(this, &UUpgradeSelectionUI::OpenUpgradeChoicesPanel);
	UpgradeSelectionFadeOutDelegate.BindDynamic(this, &UUpgradeSelectionUI::UpgradeSelectionPanelFadeOutFinished);
	GoBackAnimFinishedDelegate.BindDynamic(this, &UUpgradeSelectionUI::GoBackAnimFinished);
	CardOptions = { CardOption1, CardOption2, CardOption3, CardOption4 };
	CardOption1->GetUpgradeButton()->OnClicked.AddDynamic(this, &UUpgradeSelectionUI::TriggerSelectAnimForFirstOption);
	CardOption2->GetUpgradeButton()->OnClicked.AddDynamic(this, &UUpgradeSelectionUI::TriggerSelectAnimForSecondOption);
	CardOption3->GetUpgradeButton()->OnClicked.AddDynamic(this, &UUpgradeSelectionUI::TriggerSelectAnimForThirdOption);
	CardOption4->GetUpgradeButton()->OnClicked.AddDynamic(this, &UUpgradeSelectionUI::TriggerSelectAnimForFourthOption);
	BackButton->Get()->OnClicked.AddDynamic(this, &UUpgradeSelectionUI::GoBack);
}

void UUpgradeSelectionUI::OpenUpgradeChoicesPanel(const FRuntimeUpgradeData& InUpgradeData)
{
	if (!UpgradeChoicesPanel) return;
	if (!GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetIsExplanationShown(EExplanation::UpgradeChoiceUI))
	{
		(void)GetWorld()->GetSubsystem<UWidgetManager>()->GetOnShowExplanationDelegate().ExecuteIfBound(EExplanation::UpgradeChoiceUI, GetGameInstance());
	}
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::UpgradeChoicesFadeIn);
	bChoicesOpen = true;
	BackButton->SetButtonVisibility(true);
	for (int32 i = 0; i < UpgradesData.Num(); i++) CardOptions[i]->GetUpgradeButton()->SetIsEnabled(false);
	UpgradeChoice1->SetButtonVisibility(true);
	UpgradeChoice2->SetButtonVisibility(true);
	UpgradeChoice3->SetButtonVisibility(true);
	SetupUpgradeChoices(InUpgradeData);
	UpgradeChoicesPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UUpgradeSelectionUI::PlayAnimByOptionSelected(const bool bIncludeBindToFinish)
{
	switch (OptionSelected)
	{
		case 0:
			PlayAnimationReverse(UpgradeSelectFirstAnim); 
			if (bIncludeBindToFinish) BindToAnimationFinished(UpgradeSelectFirstAnim, GoBackAnimFinishedDelegate);
			break;
		case 1:
			PlayAnimationReverse(UpgradeSelectSecondAnim);
			if (bIncludeBindToFinish) BindToAnimationFinished(UpgradeSelectSecondAnim, GoBackAnimFinishedDelegate);
			break;
		case 2:
			PlayAnimationReverse(UpgradeSelectThirdAnim);
			if (bIncludeBindToFinish) BindToAnimationFinished(UpgradeSelectThirdAnim, GoBackAnimFinishedDelegate);
			break;
		case 3:
			PlayAnimationReverse(UpgradeSelectFourthAnim);
			if (bIncludeBindToFinish) BindToAnimationFinished(UpgradeSelectFourthAnim, GoBackAnimFinishedDelegate);
		default: ;
	}
}

void UUpgradeSelectionUI::AdjustNeededCounts(int32& InNeededCommon, int32& InNeededRare, int32& InNeededLegendary, const FRuntimeUpgradeData& InUpgradeData)
{
	const int32 CommonCap = InUpgradeData.GetMaxRarityChoicesCount(EUpgradeChoice::Common);
	const int32 RareCap = InUpgradeData.GetMaxRarityChoicesCount(EUpgradeChoice::Rare);
	const int32 LegendaryCap = InUpgradeData.GetMaxRarityChoicesCount(EUpgradeChoice::Legendary);
	InNeededCommon = FMath::Min(1, CommonCap);
	InNeededRare = FMath::Min(1, RareCap);
	InNeededLegendary = FMath::Min(1, LegendaryCap);
	int32 Available = 3 - (InNeededCommon + InNeededRare + InNeededLegendary);
	if (Available <= 0) return;
	while (Available > 0 && InNeededRare < RareCap) { ++InNeededRare; --Available; }
	while (Available > 0 && InNeededCommon < CommonCap) { ++InNeededCommon; --Available; }
	while (Available > 0 && InNeededLegendary < LegendaryCap) { ++InNeededLegendary; --Available; }
}

void UUpgradeSelectionUI::FillUpgrades(const FRuntimeUpgradeData& InUpgradeData, TArray<FUpgradeChoicePair>& OutChosen, const EUpgradeChoice InRarity, const int32 InCount)
{
	TArray<FUpgradeChoicePair> LocalChosen;
	for (int32 i = 0; i < InCount; i++)
	{
		FUpgradeChoicePair Pair = InUpgradeData.GetRandomUpgradeChoice(InRarity, LocalChosen);
		if (!Pair.IsEmpty())
		{
			OutChosen.Add(Pair);
			LocalChosen.Add(Pair);
		}
		else break;
	}
}

void UUpgradeSelectionUI::SetupUpgradeChoices(const FRuntimeUpgradeData& InUpgradeData)
{
	if (const FCachedPairs* CachedPairsData = CachedPairs.Find(InUpgradeData.GetUpgradeName()))
	{
		if (CachedPairsData->IsValidIndex(0)) UpgradeChoice1->Setup(InUpgradeData.GetUpgradeName(), CachedPairsData->Pairs[0]);
		else UpgradeChoice1->Setup(InUpgradeData.GetUpgradeName());
		if (CachedPairsData->IsValidIndex(1)) UpgradeChoice2->Setup(InUpgradeData.GetUpgradeName(), CachedPairsData->Pairs[1]);
		else UpgradeChoice2->Setup(InUpgradeData.GetUpgradeName());
		if (CachedPairsData->IsValidIndex(2)) UpgradeChoice3->Setup(InUpgradeData.GetUpgradeName(), CachedPairsData->Pairs[2]);
		else UpgradeChoice3->Setup(InUpgradeData.GetUpgradeName());
		return;
	}
	int32 NeededCommon = 1;
	int32 NeededRare = 1;
	int32 NeededLegend = 1;
	AdjustNeededCounts(NeededCommon, NeededRare, NeededLegend, InUpgradeData);
	TArray<FUpgradeChoicePair> Chosen;
	FillUpgrades(InUpgradeData, Chosen, EUpgradeChoice::Legendary, NeededLegend);
	FillUpgrades(InUpgradeData, Chosen, EUpgradeChoice::Rare, NeededRare);
	FillUpgrades(InUpgradeData, Chosen, EUpgradeChoice::Common, NeededCommon);
	CachedPairs.Add(InUpgradeData.GetUpgradeName(), FCachedPairs(Chosen));
	if (Chosen.IsValidIndex(0)) UpgradeChoice1->Setup(InUpgradeData.GetUpgradeName(), Chosen[0]);
	else UpgradeChoice1->Setup(InUpgradeData.GetUpgradeName());
	if (Chosen.IsValidIndex(1)) UpgradeChoice2->Setup(InUpgradeData.GetUpgradeName(), Chosen[1]);
	else UpgradeChoice2->Setup(InUpgradeData.GetUpgradeName());
	if (Chosen.IsValidIndex(2)) UpgradeChoice3->Setup(InUpgradeData.GetUpgradeName(), Chosen[2]);
	else UpgradeChoice3->Setup(InUpgradeData.GetUpgradeName());
}

void UUpgradeSelectionUI::TriggerSelectAnimForFirstOption()
{
	OptionSelected = 0;
	PlayAnimation(UpgradeSelectFirstAnim);
}

void UUpgradeSelectionUI::TriggerSelectAnimForSecondOption()
{
	OptionSelected = 1;
	PlayAnimation(UpgradeSelectSecondAnim);
}

void UUpgradeSelectionUI::TriggerSelectAnimForThirdOption()
{
	OptionSelected = 2;
	PlayAnimation(UpgradeSelectThirdAnim);
}

void UUpgradeSelectionUI::TriggerSelectAnimForFourthOption()
{
	OptionSelected = 3;
	PlayAnimation(UpgradeSelectFourthAnim);
}

void UUpgradeSelectionUI::UpgradeSelectionPanelFadeOutFinished()
{
	OptionSelected = -1;
	CachedPairs.Empty();
	UnbindAllFromAnimationFinished(UpgradeMenuCloseAnim);
	CloseUI();
	(void)DelegateManager->OnUpgradeSelectionFinished.ExecuteIfBound();
}

void UUpgradeSelectionUI::GoBackAnimFinished()
{
	switch (OptionSelected)
	{
		case 0: UnbindFromAnimationFinished(UpgradeSelectFirstAnim, GoBackAnimFinishedDelegate); break;
		case 1: UnbindFromAnimationFinished(UpgradeSelectSecondAnim, GoBackAnimFinishedDelegate); break;
		case 2: UnbindFromAnimationFinished(UpgradeSelectThirdAnim, GoBackAnimFinishedDelegate); break;
		case 3: UnbindFromAnimationFinished(UpgradeSelectFourthAnim, GoBackAnimFinishedDelegate);
		default: ;
	}
	OptionSelected = -1;
	for (int32 i = 0; i < UpgradesData.Num(); i++) CardOptions[i]->GetUpgradeButton()->SetIsEnabled(true);
}

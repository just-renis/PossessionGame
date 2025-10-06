#include "DemoEndUI.h"
#include "PlayerHUD.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/UI/SecondaryWidgets/MainMenuButtonUI.h"
#include "PossessionGame/Utilities/Data Utilities/FLoadLevelData.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

int32 UDemoEndUI::GetRankCoinsAmount()
{
	if (Ranks.IsEmpty()) return 1;
	for (int32 i = 0; i < Ranks.Num(); i++)
	{
		if (RunEndTime <= Ranks[i].TimeInSecondsMax) return Ranks.Num() - i;
	}
	return 1;
}

FString UDemoEndUI::GetConvertedNumber(const int32 InNumber)
{
	if (InNumber < 10) return TEXT("0") + FString::FromInt(InNumber);
	return FString::FromInt(InNumber);
}

void UDemoEndUI::NativeConstruct()
{
	Super::NativeConstruct();
	bIsArenaMode = GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestIsArenaMode.Execute();
	ChapterCompletedText->SetText(bIsArenaMode ? NSLOCTEXT("ST_Localization", "Arena", "Arena") : NSLOCTEXT("ST_Localization", "ChapterCompleted", "Chapter Completed"));
	FadeFinishedDelegate.BindDynamic(this, &UDemoEndUI::FadeInFinished);
	if (bIsArenaMode)
	{
		RankBonusBox->SetVisibility(ESlateVisibility::Collapsed);
		BindToAnimationFinished(FadeInArenaAnim, FadeFinishedDelegate);
		PlayAnimation(FadeInArenaAnim);
	}
	else
	{
		HighScoreEndTimeText->SetVisibility(ESlateVisibility::Collapsed);
		BindToAnimationFinished(FadeInAnim, FadeFinishedDelegate);
		PlayAnimation(FadeInAnim);
	}
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0.f, 1.f, bIsArenaMode ? 1.f : 0.25f, FLinearColor::Black, false, true);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = true;
	PlayerController->SetInputMode(FInputModeGameAndUI());
	GetWorld()->GetTimerManager().SetTimer(PauseHandle, this, &UDemoEndUI::PauseGame, 0.25f);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPlayerHUD>(EWidget::PlayerHUD)->PlayFadeOutAnim(true);
	SetupRankUI();
}

void UDemoEndUI::PauseGame() const
{
	UGameplayStatics::SetGamePaused(GetWorld(), true);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::EndScreenFadeIn);
}

void UDemoEndUI::SetupRankUI()
{
	if (!TimeText || !CoinsAmountText) return;
	RunEndTime = GetGameInstance()->GetSubsystem<UDelegateManager>()->OnRunFinished.Execute();
	SetRankTexture();
	if (!bIsArenaMode)
	{
		const int32 CoinsAmount = GetRankCoinsAmount();
		CoinsAmountText->SetText(FText::AsNumber(CoinsAmount));
		GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->AddCurrencyAmount(CoinsAmount);
	}
	else
	{
		GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->AddArenaStats(RunEndTime);
		SetTimeText(HighScoreEndTimeText, GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetArenaMaxTimeSurvivedSeconds(), NSLOCTEXT("ST_Localization", "HighScore", "High Score"));
	}
	SetTimeText(TimeText, RunEndTime, NSLOCTEXT("ST_Localization", "Time", "Time"));
}

void UDemoEndUI::SetRankTexture()
{
	if (!RankImage || Ranks.IsEmpty()) return;
	for (const FRunTimeRankData RankData : Ranks)
	{
		if (bIsArenaMode)
		{
			if (RunEndTime >= RankData.ArenaTimeInSecondsMin)
			{
				RankImage->SetBrushFromAtlasInterface(RankData.RankTexture);
				RankImage->SetDesiredSizeOverride(FVector2D(200.f));
				return;
			}
		}
		else
		{
			if (RunEndTime <= RankData.TimeInSecondsMax)
			{
				RankImage->SetBrushFromAtlasInterface(RankData.RankTexture);
				RankImage->SetDesiredSizeOverride(FVector2D(200.f));
				return;
			}
		}
	}
	RankImage->SetBrushFromAtlasInterface(Ranks.Last().RankTexture);
	RankImage->SetDesiredSizeOverride(FVector2D(200.f));
}

void UDemoEndUI::SetTimeText(const TObjectPtr<UTextBlock>& InTextBlock, const int32 InTime, const FText& InText)
{
	const int32 Hours = InTime / 3600;
	const int32 Minutes = InTime % 3600 / 60;
	const int32 Seconds = InTime % 60;
	InTextBlock->SetText(FText::FromString(InText.ToString() + TEXT(" ") + GetConvertedNumber(Hours) + TEXT(":") + GetConvertedNumber(Minutes) + TEXT(":") + GetConvertedNumber(Seconds)));
}

void UDemoEndUI::FadeInFinished()
{
	if (ReturnToMenuButton)
	{
		ReturnToMenuButton->SetButtonVisibility(true);
		ReturnToMenuButton->Get()->OnClicked.AddDynamic(this, &UDemoEndUI::ReturnToMainMenu);
	}
	if (ReplayButton)
	{
		ReplayButton->SetButtonVisibility(true);
		ReplayButton->Get()->OnClicked.AddDynamic(this, &UDemoEndUI::Replay);
	}
	UnbindFromAnimationFinished(bIsArenaMode ? FadeInArenaAnim : FadeInAnim, FadeFinishedDelegate);
}

void UDemoEndUI::ReturnToMainMenu()
{
	if (bAlreadyClickedButton) return;
	bAlreadyClickedButton = true;
	LevelNameToLoad = FName("MainMenu");
	FadeFinishedDelegate.BindDynamic(this, &UDemoEndUI::FadeOutToNextLevel);
	PlayAnimation(FadeOutAnim);
	BindToAnimationFinished(FadeOutAnim, FadeFinishedDelegate);
}

void UDemoEndUI::Replay()
{
	if (bAlreadyClickedButton) return;
	bAlreadyClickedButton = true;
	LevelNameToLoad = bIsArenaMode ? FName("L_Arena") : FName("Level_1");
	FadeFinishedDelegate.BindDynamic(this, &UDemoEndUI::FadeOutToNextLevel);
	PlayAnimation(FadeOutAnim);
	BindToAnimationFinished(FadeOutAnim, FadeFinishedDelegate);
}

void UDemoEndUI::FadeOutToNextLevel()
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->LoadLevel.ExecuteIfBound(FLoadLevelData(false, false, true, LevelNameToLoad));
}

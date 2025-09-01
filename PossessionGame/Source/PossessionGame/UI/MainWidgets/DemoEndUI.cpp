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
	FadeFinishedDelegate.BindDynamic(this, &UDemoEndUI::FadeInFinished);
	BindToAnimationFinished(FadeInAnim, FadeFinishedDelegate);
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0.f, 1.f, 0.25f, FLinearColor::Black, false, true);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = true;
	PlayerController->SetInputMode(FInputModeGameAndUI());
	PlayAnimation(FadeInAnim);
	GetWorld()->GetTimerManager().SetTimer(PauseHandle, this, &UDemoEndUI::PauseGame, 0.25f);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPlayerHUD>(EWidget::PlayerHUD)->PlayFadeOutAnim(true);
	SetupRankUI();
}

void UDemoEndUI::PauseGame() const { UGameplayStatics::SetGamePaused(GetWorld(), true); }

void UDemoEndUI::SetupRankUI()
{
	if (!TimeText || !CoinsAmountText) return;
	RunEndTime = GetGameInstance()->GetSubsystem<UDelegateManager>()->OnRunFinished.Execute();
	SetRankTexture();
	const int32 CoinsAmount = GetRankCoinsAmount();
	CoinsAmountText->SetText(FText::AsNumber(CoinsAmount));
	GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->AddCurrencyAmount(CoinsAmount);
	const int32 Hours = RunEndTime / 3600;
	const int32 Minutes = RunEndTime % 3600 / 60;
	const int32 Seconds = RunEndTime % 60;
	TimeText->SetText(FText::FromString(NSLOCTEXT("ST_Localization", "Time", "Time").ToString() + TEXT(" ") + GetConvertedNumber(Hours) + TEXT(":") + GetConvertedNumber(Minutes) + TEXT(":") + GetConvertedNumber(Seconds)));
}

void UDemoEndUI::SetRankTexture()
{
	if (!RankImage || Ranks.IsEmpty()) return;
	for (const FRunTimeRankData RankData : Ranks)
	{
		if (RunEndTime <= RankData.TimeInSecondsMax)
		{
			RankImage->SetBrushFromAtlasInterface(RankData.RankTexture);
			return;
		}
	}
	RankImage->SetBrushFromAtlasInterface(Ranks.Last().RankTexture);
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
	UnbindFromAnimationFinished(FadeInAnim, FadeFinishedDelegate);
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
	LevelNameToLoad = FName("Level_1");
	FadeFinishedDelegate.BindDynamic(this, &UDemoEndUI::FadeOutToNextLevel);
	PlayAnimation(FadeOutAnim);
	BindToAnimationFinished(FadeOutAnim, FadeFinishedDelegate);
}

void UDemoEndUI::FadeOutToNextLevel()
{
	GetWorld()->GetTimerManager().ClearTimer(PauseHandle);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->LoadLevel.ExecuteIfBound(FLoadLevelData(false, false, true, LevelNameToLoad));
}

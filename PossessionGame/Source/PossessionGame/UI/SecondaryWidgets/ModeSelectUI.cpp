#include "ModeSelectUI.h"
#include "MainMenuButtonUI.h"
#include "Components/Button.h"
#include "PossessionGame/UI/MainWidgets/SaveSlotMenuUI.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

TObjectPtr<UButton> UModeSelectUI::GetBackButton() const { return BackButton->Get(); }

void UModeSelectUI::PlayFadeInAnim(const bool bState, const FLoadLevelData& InLevelData)
{
	if (InLevelData.LevelName != NAME_None) PendingLoadLevelData = InLevelData;
	PlayAnimation(FadeInAnim, GetAnimationCurrentTime(FadeInAnim), 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(bState ? ESound::PanelFadeIn : ESound::PanelFadeOut);
	if (bState) SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	else BindToAnimationFinished(FadeInAnim, FadeOutDelegate);
	if (StoryModeButton) StoryModeButton->SetButtonVisibility(bState);
	if (ArenaModeButton) ArenaModeButton->SetButtonVisibility(bState);
	if (BackButton) BackButton->SetButtonVisibility(bState);
}

void UModeSelectUI::NativeConstruct()
{
	Super::NativeConstruct();
	StoryModeButton->Get()->OnClicked.AddDynamic(this, &UModeSelectUI::OpenStoryMode);
	ArenaModeButton->Get()->OnClicked.AddDynamic(this, &UModeSelectUI::OpenArenaMode);
	FadeOutDelegate.BindDynamic(this, &UModeSelectUI::FadeOutAnimFinished);
}

void UModeSelectUI::DisablePlayerInput() const
{
	if (const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController())
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->DisableInput(PlayerController);
		PlayerController->bShowMouseCursor = false;
	}
}

void UModeSelectUI::LoadLevel() const { (void)GetGameInstance()->GetSubsystem<UDelegateManager>()->LoadLevel.ExecuteIfBound(PendingLoadLevelData); }

void UModeSelectUI::OpenStoryMode()
{
	DisablePlayerInput();
	PlayFadeInAnim(false);
	ParentWidget->PlayBlackFadeOutAnim();
	GetWorld()->GetTimerManager().SetTimer(LoadLevelHandle, this, &UModeSelectUI::LoadLevel, 1.5f);
}

void UModeSelectUI::OpenArenaMode()
{
	DisablePlayerInput();
	PlayFadeInAnim(false);
	ParentWidget->PlayBlackFadeOutAnim();
	if (PendingLoadLevelData.LevelName != FName("TutorialLevel")) PendingLoadLevelData.LevelName = FName("L_Arena");
	GetWorld()->GetTimerManager().SetTimer(LoadLevelHandle, this, &UModeSelectUI::LoadLevel, 1.5f);
}

void UModeSelectUI::FadeOutAnimFinished()
{
	UnbindAllFromAnimationFinished(FadeInAnim);
	SetVisibility(ESlateVisibility::Collapsed);
}

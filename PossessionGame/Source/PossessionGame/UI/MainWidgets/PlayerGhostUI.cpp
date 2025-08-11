#include "PlayerGhostUI.h"

#include "PlayerHUD.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/InvalidationBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/GlobalSettingsManager.h"
#include "PossessionGame/Utilities/Subsystems/TutorialManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

bool UPlayerGhostUI::IsPressKeyShown() const { return PressKeyToContinueInv->GetVisibility() == ESlateVisibility::SelfHitTestInvisible; }

float UPlayerGhostUI::GetStartupAnimationLength() const { return StartupAnim->GetEndTime(); }

void UPlayerGhostUI::PlayStartupAnimation() { PlayAnimation(StartupAnim); }

FText UPlayerGhostUI::GetKeyBindText(const int32 InIndex) const
{
	switch (InIndex)
	{
		case 0: return GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetKeyBindText("Hijack");
		case 2: return GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetKeyBindText("Melee");
		case 3: return GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetKeyBindText("Run");
		case 4: case 5: return GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetKeyBindText("Jump");
		case 6: return GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetKeyBindText("Crouch / Slide / Stomp");
		case 8: return GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetKeyBindText("Shoot");
		default: ;
	}
	return FText();
}

void UPlayerGhostUI::NativeConstruct()
{
	Super::NativeConstruct();
	GetWorld()->GetSubsystem<UTutorialManager>()->OnShowHint.AddUObject(this, &UPlayerGhostUI::ShowHint);
	GetWorld()->GetSubsystem<UTutorialManager>()->OnStopShowingHint.BindUObject(this, &UPlayerGhostUI::StopShowingHint);
	GetWorld()->GetSubsystem<UTutorialManager>()->OnShowPressKeyToContinue.AddUObject(this, &UPlayerGhostUI::ShowPressKey);
	GetWorld()->GetSubsystem<UTutorialManager>()->TriggerTutorialEnd.BindUObject(this, &UPlayerGhostUI::PlayFadeOutAnim);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnKeyBindChange.AddUObject(this, &UPlayerGhostUI::UpdateKeyBindText);
}

void UPlayerGhostUI::ShowHint()
{
	PlayAnimation(HintAppearAnim);
	SetHintText(CurrentHintIndex++);
	if (CurrentHintIndex == 2 && MovementHintBox) MovementHintBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	else if (CurrentHintIndex == 9)
	{
		(void)GetWorld()->GetSubsystem<UTutorialManager>()->SpawnTutorialEnemy.ExecuteIfBound(ECharacterType::Minion, EGameMode::TutorialMode2);
	}
}

void UPlayerGhostUI::SetHintText(const int32 InIndex) const
{
	if (!HintInv || !HintText) return;
	HintInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	const FText KeyBindText = GetKeyBindText(InIndex);
	switch (InIndex)
	{
		case 0: HintText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "HijackRobotsHint", "[{0}] to Hijack Robots"), KeyBindText)); break;
		case 1: HintText->SetText(NSLOCTEXT("ST_Localization", "MoveHint", "Move")); break;
		case 2: HintText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "DoorBreakHint", "Press [{0}] to Break Door"), KeyBindText)); break;
		case 3: HintText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "RunHint", "[{0}] to Run"), KeyBindText)); break;
		case 4: HintText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "JumpHint", "Press [{0}] to Jump"), KeyBindText)); break;
		case 5: HintText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "DoubleJumpHint", "Double Press [{0}] to Double Jump"), KeyBindText)); break;
		case 6: HintText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "SlideHint", "Press [{0}] + [{1}] to Slide"), GetKeyBindText(3), KeyBindText)); break;
		case 7: HintText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "StompHint", "Double Press [{0}] and then [{1}] to Stomp"), GetKeyBindText(4), GetKeyBindText(6))); break;
		case 8: HintText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "ShootHint", "Press [{0}] to Shoot"), KeyBindText)); break;
		default: HintText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "HijackRobotHint", "Press [{0}] to Hijack a New Robot"), GetKeyBindText(0)));
	}
}

void UPlayerGhostUI::StopShowingHint() const
{
	if (HintInv) HintInv->SetVisibility(ESlateVisibility::Collapsed);
	if (MovementHintBox) MovementHintBox->SetVisibility(ESlateVisibility::Collapsed);
}

void UPlayerGhostUI::ShowPressKey(const bool bState)
{
	if (!PressKeyToContinueInv || !PressKeyToContinueText) return;
	PressKeyToContinueInv->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	PressKeyToContinueText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "PressKeyToContinue", "Press [{0}] Key To Continue"), GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetKeyBindText("Interact")));
	if (bState) PlayAnimation(PressKeyAppearAnim);
}

void UPlayerGhostUI::UpdateKeyBindText(const FName& InKeyBindName, const FText& InKeyBindText) const
{
	if (PressKeyToContinueText && InKeyBindName.IsEqual("Interact")) PressKeyToContinueText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "PressKeyToContinue", "Press [{0}] Key To Continue"), InKeyBindText));
	if (!HintText) return;
	const TObjectPtr<UGlobalSettingsManager> GlobalSettingsManager = GetGameInstance()->GetSubsystem<UGlobalSettingsManager>();
	switch (CurrentHintIndex)
	{
		case 1: if (InKeyBindName.IsEqual("Hijack")) HintText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "HijackRobotsHint", "[{0}] to Hijack Robots"), InKeyBindText)); break;
		case 3: if (InKeyBindName.IsEqual("Melee")) HintText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "DoorBreakHint", "Press [{0}] to Break Door"), InKeyBindText)); break;
		case 4: if (InKeyBindName.IsEqual("Run")) HintText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "RunHint", "[{0}] to Run"), InKeyBindText)); break;
		case 5: if (InKeyBindName.IsEqual("Jump")) HintText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "JumpHint", "Press [{0}] to Jump"), InKeyBindText)); break;
		case 6: if (InKeyBindName.IsEqual("Jump")) HintText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "DoubleJumpHint", "Double Press [{0}] to Double Jump"), InKeyBindText)); break;
		case 7: if (InKeyBindName.IsEqual("Crouch / Slide / Stomp")) HintText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "SlideHint", "Press [{0}] + [{1}] to Slide"), GlobalSettingsManager->GetKeyBindText("Run"), InKeyBindText)); break;
		case 8: HintText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "StompHint", "Double Press [{0}] and then [{1}] to Stomp"), GlobalSettingsManager->GetKeyBindText("Jump"), GlobalSettingsManager->GetKeyBindText("Crouch / Slide / Stomp"))); break;
		case 9: HintText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "ShootHint", "Press [{0}] to Shoot"), InKeyBindText)); break;
		default: HintText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "HijackRobotHint", "Press [{0}] to Hijack a New Robot"), GetKeyBindText(0)));
	}
}

void UPlayerGhostUI::PlayFadeOutAnim()
{
	Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot()->SetTutorialFinished();
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPlayerHUD>(EWidget::PlayerHUD)->PlayFadeOutAnim(true);
	FadeOutAnimFinishedDelegate.BindDynamic(this, &UPlayerGhostUI::LoadNextLevel);
	BindToAnimationFinished(FadeOutAnim, FadeOutAnimFinishedDelegate);
	PlayAnimation(FadeOutAnim);
}

void UPlayerGhostUI::LoadNextLevel()
{
	UnbindFromAnimationFinished(FadeOutAnim, FadeOutAnimFinishedDelegate);
	UGameplayStatics::OpenLevel(this, "Level_1");
}

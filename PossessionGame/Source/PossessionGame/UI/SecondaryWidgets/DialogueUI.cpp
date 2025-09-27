#include "DialogueUI.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Enums/EGameState.h"
#include "PossessionGame/Enums/ETutorialBotAnimState.h"
#include "PossessionGame/Enums/ETutorialBotExpression.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/TutorialManager.h"

void UDialogueUI::StartTipDialogue(const EGameState InCurrentGameState, const float InSpeed)
{
	if (GetVisibility() == ESlateVisibility::Collapsed) SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	AssetsManager->PlaySound2D(ESound::DialogueStart);
	if (TargetText.IsEmpty()) TargetText = GetDialogueText(InCurrentGameState).ToString();
	CurrentText = "";
	CurrentIndex = 0;
	TypeSpeed = InSpeed;
	GetWorld()->GetTimerManager().SetTimer(TypeHandle, this, &UDialogueUI::TypeNextLetter, TypeSpeed, true);
}

void UDialogueUI::ClearDialogueText() const { if (DialogueText) DialogueText->SetText(FText::FromString("")); }

void UDialogueUI::StartDialogue(const int32 InIndex, const bool bShowPressAnyKeyToContinue, const float InSpeed)
{
	if (!DialogueTexts.IsValidIndex(InIndex) && InIndex != -1) return;
	if (GetVisibility() == ESlateVisibility::Collapsed) SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	bShowPressAnyKeyToContinueOnDialogueEnd = bShowPressAnyKeyToContinue;
	AssetsManager->PlaySound2D(ESound::DialogueStart);
	TargetText = DialogueTexts[InIndex].ToString();
	CurrentText = "";
	CurrentIndex = 0;
	TypeSpeed = InSpeed;
	GetWorld()->GetTimerManager().SetTimer(TypeHandle, this, &UDialogueUI::TypeNextLetter, TypeSpeed, true);
}

void UDialogueUI::SetNextDialogueText(const EGameState InCurrentGameState) { TargetText = GetDialogueText(InCurrentGameState).ToString(); }

void UDialogueUI::FinishedTypingDialogue()
{
	GetWorld()->GetTimerManager().ClearTimer(TypeHandle);
	if (bShowPressAnyKeyToContinueOnDialogueEnd)
	{
		GetWorld()->GetSubsystem<UTutorialManager>()->OnShowPressKeyToContinue.Broadcast(true);
		bShowPressAnyKeyToContinueOnDialogueEnd = false;
	}
}

FText UDialogueUI::GetDialogueText(const EGameState InGameState)
{
	if (const FGameStateDialogues* GameStateDialoguesData = TipTexts.Find(InGameState))
	{
		TMap<ETutorialBotExpression, FExpressionDialogues> GameStateDialogues = GameStateDialoguesData->Texts;
		if (GameStateDialogues.IsEmpty()) return FText();
		TArray<ETutorialBotExpression> Keys;
		GameStateDialogues.GetKeys(Keys);
		const ETutorialBotExpression Expression = Keys[FMath::RandRange(0, Keys.Num() - 1)];
		const FExpressionDialogues& RandomValue = GameStateDialogues[Expression];
		(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTutorialBotTipDialogue.ExecuteIfBound(RandomValue.Animation, Expression);
		if (RandomValue.Texts.IsEmpty()) return FText();
		return RandomValue.Texts[FMath::RandRange(0, RandomValue.Texts.Num() - 1)];
	}
	return FText();
}

void UDialogueUI::NativeConstruct()
{
	Super::NativeConstruct();
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue0", "Owch."));
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue1", "Hi! My name is Bot, I'm here to get you out of here."));
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue2", "Ok, let's get you a body first."));
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue3", "See this body? Look at it and hijack it!"));
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue4", "Nice!"));
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue5", "Damn it!"));
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue6", "It's not safe for us here."));
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue7", "Down the hall further is a core room we're we can safely keep as our base."));
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue8", "I'll go through the vents, you follow the path."));
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue9", "Break these doors and get going."));
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue10", "We're almost there. Keep going!"));
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue11", "You made it!"));
	DialogueTexts.Add(NSLOCTEXT("ST_Localization", "TutorialDialogue12", "Now let's set everything up."));
	
	FGameStateDialogues GameStateDialogues;
	FExpressionDialogues ExpressionDialogues;
	ExpressionDialogues.Animation = ETutorialBotAnimState::TabletHolding;
	ExpressionDialogues.Texts.Add(NSLOCTEXT("ST_Localization", "PreGameTip0", "Don't forget to setup your abilities."));
	ExpressionDialogues.Texts.Add(NSLOCTEXT("ST_Localization", "PreGameTip1", "Let's setup your abilities."));
	ExpressionDialogues.Texts.Add(NSLOCTEXT("ST_Localization", "PreGameTip2", "Don't forget your abilities."));
	GameStateDialogues.Texts.Add(ETutorialBotExpression::NeutralFace, ExpressionDialogues);
	TipTexts.Add(EGameState::PreGame, GameStateDialogues);
	GameStateDialogues.Texts.Empty();
	ExpressionDialogues.Texts.Empty();

	ExpressionDialogues.Animation = ETutorialBotAnimState::Dancing;
	ExpressionDialogues.Texts.Add(NSLOCTEXT("ST_Localization", "DuringGameDancingTip0", "Good Job!"));
	ExpressionDialogues.Texts.Add(NSLOCTEXT("ST_Localization", "DuringGameDancingTip1", "Nice Shooting!"));
	ExpressionDialogues.Texts.Add(NSLOCTEXT("ST_Localization", "DuringGameDancingTip2", "Eliminating enemies charges your hijack bar."));
	ExpressionDialogues.Texts.Add(NSLOCTEXT("ST_Localization", "DuringGameDancingTip3", "Robots have limited amount of ammo, make sure you don't run out."));
	GameStateDialogues.Texts.Add(ETutorialBotExpression::Happy, ExpressionDialogues);
	ExpressionDialogues.Texts.Empty();
	ExpressionDialogues.Animation = ETutorialBotAnimState::Thinking;
	ExpressionDialogues.Texts.Add(NSLOCTEXT("ST_Localization", "DuringGameThinkingTip0", "I think there's someone else fighting with us."));
	ExpressionDialogues.Texts.Add(NSLOCTEXT("ST_Localization", "DuringGameThinkingTip1", "My radar detects we have an ally nearby."));
	ExpressionDialogues.Texts.Add(NSLOCTEXT("ST_Localization", "DuringGameThinkingTip2", "Something scary is happening in the world, unknown entities."));
	ExpressionDialogues.Texts.Add(NSLOCTEXT("ST_Localization", "DuringGameThinkingTip3", "The only way to get out of here is to go up."));
	ExpressionDialogues.Texts.Add(NSLOCTEXT("ST_Localization", "DuringGameThinkingTip4", "Earth seems to be having unidentified guests from space."));
	GameStateDialogues.Texts.Add(ETutorialBotExpression::Thinking, ExpressionDialogues);
	TipTexts.Add(EGameState::DuringGame, GameStateDialogues);
	GameStateDialogues.Texts.Empty();
	ExpressionDialogues.Texts.Empty();
	
	ExpressionDialogues.Animation = ETutorialBotAnimState::Idle;
	ExpressionDialogues.Texts.Add(NSLOCTEXT("ST_Localization", "PreBossTip0", "This fight will be tough! Good Luck!"));
	ExpressionDialogues.Texts.Add(NSLOCTEXT("ST_Localization", "PreBossTip1", "A strong enemy awaits further ahead."));
	ExpressionDialogues.Texts.Add(NSLOCTEXT("ST_Localization", "PreBossTip2", "Tough guy up next. Give 'em hell!"));
	GameStateDialogues.Texts.Add(ETutorialBotExpression::NeutralFace, ExpressionDialogues);
	TipTexts.Add(EGameState::PreBoss, GameStateDialogues);
	GameStateDialogues.Texts.Empty();
	ExpressionDialogues.Texts.Empty();
	
	ExpressionDialogues.Animation = ETutorialBotAnimState::Celebration;
	ExpressionDialogues.Texts.Add(NSLOCTEXT("ST_Localization", "PostBossTip0", "You did it! You were awesome back there!"));
	ExpressionDialogues.Texts.Add(NSLOCTEXT("ST_Localization", "PostBossTip1", "That big guy stood no chance againts you!"));
	ExpressionDialogues.Texts.Add(NSLOCTEXT("ST_Localization", "PostBossTip2", "Awesome shooting, that robot was no match for you!"));
	GameStateDialogues.Texts.Add(ETutorialBotExpression::Happy, ExpressionDialogues);
	TipTexts.Add(EGameState::PostBoss, GameStateDialogues);
	GameStateDialogues.Texts.Empty();
	ExpressionDialogues.Texts.Empty();
	
	ExpressionDialogues.Animation = ETutorialBotAnimState::FingerPointing;
	ExpressionDialogues.Texts.Add(NSLOCTEXT("ST_Localization", "SecretRoomTip0", "You found an Artifact, have a look inside!"));
	ExpressionDialogues.Texts.Add(NSLOCTEXT("ST_Localization", "SecretRoomTip1", "You found the purple coin, it's a key for these gates."));
	ExpressionDialogues.Texts.Add(NSLOCTEXT("ST_Localization", "SecretRoomTip2", "You found the key, have a look inside."));
	GameStateDialogues.Texts.Add(ETutorialBotExpression::NeutralFace, ExpressionDialogues);
	TipTexts.Add(EGameState::SecretRoom, GameStateDialogues);
	GameStateDialogues.Texts.Empty();
	ExpressionDialogues.Texts.Empty();
	
	DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	AssetsManager = GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>();
    if (DialogueText) DialogueText->SetText(FText::FromString(""));
}

void UDialogueUI::TypeNextLetter()
{
	if (!DialogueText) return;
	if (CurrentIndex < TargetText.Len())
	{
		CurrentText.AppendChar(TargetText[CurrentIndex++]);
		DialogueText->SetText(FText::FromString(CurrentText));
	}
	else FinishedTypingDialogue();
}

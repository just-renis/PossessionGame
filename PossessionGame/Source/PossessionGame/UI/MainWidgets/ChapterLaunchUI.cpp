#include "ChapterLaunchUI.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

FText UChapterLaunchUI::GetChapterName(const int32 InCurrentChapter)
{
	switch (InCurrentChapter)
	{
		case 1: return NSLOCTEXT("ST_Localization", "DeepLabs", "Deep Labs");
		case 2: return NSLOCTEXT("ST_Localization", "Rustline", "Rustline");
		default: return FText();
	}
}

void UChapterLaunchUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestIsArenaMode.Execute()) SetTexts(NSLOCTEXT("ST_Localization", "ArenaMode", "Arena Mode"), FText());
	else
	{
		const int32 CurrentChapter = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetCurrentChapter();
		SetTexts(FText::Format(NSLOCTEXT("ST_Localization", "Chapter", "Chapter {0}"), FText::AsNumber(CurrentChapter)), GetChapterName(CurrentChapter));
	}
	FadeInAnimFinishedDelegate.BindDynamic(this, &UChapterLaunchUI::FadeInAnimFinished);
	BindToAnimationFinished(FadeInAnim, FadeInAnimFinishedDelegate);
	PlayAnimation(FadeInAnim);
}

void UChapterLaunchUI::SetTexts(const FText& InChapterText, const FText& InTitleText) const
{
	ChapterText->SetText(InChapterText);
	TitleText->SetText(InTitleText);
}

void UChapterLaunchUI::FadeInAnimFinished()
{
	UnbindFromAnimationFinished(FadeInAnim, FadeInAnimFinishedDelegate);
	GetWorld()->GetSubsystem<UWidgetManager>()->DestroyWidget(EWidget::ChapterLaunchUI);
}

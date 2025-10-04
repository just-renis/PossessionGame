#include "ChapterLaunchUI.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UChapterLaunchUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestIsArenaMode.Execute()) SetTexts(NSLOCTEXT("ST_Localization", "ArenaMode", "Arena Mode"), FText());
	else SetTexts(NSLOCTEXT("ST_Localization", "Chapter1", "Chapter 1"), NSLOCTEXT("ST_Localization", "DeepLabs", "Deep Labs"));
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

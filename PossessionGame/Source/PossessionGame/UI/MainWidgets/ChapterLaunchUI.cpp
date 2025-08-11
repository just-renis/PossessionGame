#include "ChapterLaunchUI.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UChapterLaunchUI::NativeConstruct()
{
	Super::NativeConstruct();
	FadeInAnimFinishedDelegate.BindDynamic(this, &UChapterLaunchUI::FadeInAnimFinished);
	BindToAnimationFinished(FadeInAnim, FadeInAnimFinishedDelegate);
	PlayAnimation(FadeInAnim);
}

void UChapterLaunchUI::FadeInAnimFinished()
{
	UnbindFromAnimationFinished(FadeInAnim, FadeInAnimFinishedDelegate);
	GetWorld()->GetSubsystem<UWidgetManager>()->DestroyWidget(EWidget::ChapterLaunchUI);
}

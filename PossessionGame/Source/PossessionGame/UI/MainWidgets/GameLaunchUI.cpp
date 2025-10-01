#include "GameLaunchUI.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UGameLaunchUI::NativeConstruct()
{
	Super::NativeConstruct();
	GameLaunchAnimEndDelegate.BindDynamic(this, &UGameLaunchUI::GameLaunchAnimationFinished);
	BindToAnimationFinished(GameLaunchAnimation, GameLaunchAnimEndDelegate);
	PlayAnimation(GameLaunchAnimation);
}

void UGameLaunchUI::GameLaunchAnimationFinished()
{
	UnbindAllFromAnimationFinished(GameLaunchAnimation);
	GetWorld()->GetSubsystem<UWidgetManager>()->DestroyWidget(EWidget::GameLaunchUI);
}

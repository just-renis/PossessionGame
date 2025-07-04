#include "GameLaunchUI.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
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
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGameLaunchFinished.ExecuteIfBound();
	UnbindAllFromAnimationFinished(GameLaunchAnimation);
	GetWorld()->GetSubsystem<UWidgetManager>()->DestroyWidget(EWidget::GameLaunchUI);
}

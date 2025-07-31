#include "TutorialManager.h"
#include "DelegateManager.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"

void UTutorialManager::Init()
{
	bIsInitialized = true;
	(void)GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->StartTutorialBotAnimation.ExecuteIfBound();
	GetWorld()->GetTimerManager().SetTimer(TutorialHandle, this, &UTutorialManager::CheckHijacked, 0.05f, true);
}

void UTutorialManager::CheckHijacked()
{
	if (!Player || !Player->GetIsHijacked()) return;
	(void)GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->OnShowHint.ExecuteIfBound();
	LastPlayerLocation = Player->GetActorLocation();
	GetWorld()->GetTimerManager().SetTimer(TutorialHandle, this, &UTutorialManager::CheckMovement, 0.05f, true);
}

void UTutorialManager::CheckMovement()
{
	if (!Player) return;
	if (!Player->GetActorLocation().Equals(LastPlayerLocation, 0.1f))
	{
		GetWorld()->GetTimerManager().SetTimer(MoveTutorialDisappearHandle, this, &UTutorialManager::StopShowingHint, 4.f);
		GetWorld()->GetTimerManager().ClearTimer(TutorialHandle);
	}
}

void UTutorialManager::StopShowingHint()
{
	(void)GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->StopShowingHint.ExecuteIfBound();
}

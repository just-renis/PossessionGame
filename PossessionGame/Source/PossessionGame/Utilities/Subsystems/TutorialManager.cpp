#include "TutorialManager.h"
#include "DelegateManager.h"
#include "InputMappingContext.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"

void UTutorialManager::Init()
{
	bIsInitialized = true;
	(void)GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->StartTutorialBotAnimation.ExecuteIfBound();
	(void)GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->OnShowPressKeyToContinue.AddUObject(this, &UTutorialManager::SetPressKeyState);
	GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTutorialMinionDeath.BindUObject(this, &UTutorialManager::MinionDeathOccured);
	GetWorld()->GetTimerManager().SetTimer(TutorialHandle, this, &UTutorialManager::CheckHijacked, 0.05f, true);
}

void UTutorialManager::SetPlayerRef(const TObjectPtr<AEnemy>& InPlayer)
{
	Player = InPlayer;
	if (Player->InputComponent)
	{
		if (const TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = Cast<UEnhancedInputComponent>(Player->InputComponent))
		{
			EnhancedInputComponent->BindAction(Player->GetMappingContext()->GetMapping(17).Action, ETriggerEvent::Started, this, &UTutorialManager::PressKeyToContinue);
		}
	}
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
		GetWorld()->GetTimerManager().SetTimer(MoveTutorialDisappearHandle, this, &UTutorialManager::StopShowingHint, 2.f);
		GetWorld()->GetTimerManager().ClearTimer(TutorialHandle);
	}
}

void UTutorialManager::StopShowingHint() const
{
	(void)GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->StopShowingHint.ExecuteIfBound();
}

void UTutorialManager::PressKeyToContinue()
{
	if (!bPressKeyEnabled) return;
	bPressKeyEnabled = false;
	GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->OnShowPressKeyToContinue.Broadcast(false);
}

void UTutorialManager::MinionDeathOccured()
{
	if (--MinionDeathsRequired <= 0)
	{
		(void)OnMinionDeathsCompleted.ExecuteIfBound();
		(void)GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->StopShowingHint.ExecuteIfBound();
		GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTutorialMinionDeath.Unbind();
	}
}

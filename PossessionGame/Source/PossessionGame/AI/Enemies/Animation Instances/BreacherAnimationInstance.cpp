#include "BreacherAnimationInstance.h"
#include "PossessionGame/AI/Enemies/Controllers/BreacherAIController.h"

void UBreacherAnimationInstance::OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted)
{
	Super::OnMontageEnd(InMontage, bIsInterrupted);
	const TObjectPtr<UAnimMontage>* BashMontage = Montages.Find(EEnemyAnimState::Bash);
	if (BashMontage && InMontage == BashMontage->Get())
	{
		UE_LOG(LogTemp, Error, TEXT("FINISHED BASH"));
		Proprietor->SetIsActionInMotion(false);
		(void)Cast<ABreacherAIController>(Proprietor->GetAI())->OnBashFinished.ExecuteIfBound();
	}
}

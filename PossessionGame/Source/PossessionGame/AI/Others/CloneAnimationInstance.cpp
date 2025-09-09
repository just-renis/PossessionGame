#include "CloneAnimationInstance.h"
#include "Clone.h"
#include "KismetAnimationLibrary.h"

void UCloneAnimationInstance::SetIsEnabled(const bool bState)
{
	bEnabled = bState;
	if (bState) GetWorld()->GetTimerManager().SetTimer(UpdateDirectionalAngleHandle, this, &UCloneAnimationInstance::UpdateDirectionalAngle, 0.05f, true);
	else GetWorld()->GetTimerManager().ClearTimer(UpdateDirectionalAngleHandle);
}

void UCloneAnimationInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	Proprietor = Cast<AClone>(GetOwningActor());
}

void UCloneAnimationInstance::UpdateDirectionalAngle()
{
	if (!Proprietor) return;
	const FVector CurrentVelocity = Proprietor->GetVelocity();
	if (!CurrentVelocity.Equals(LastVelocity, 1.f))
	{
		DirectionalAngle = UKismetAnimationLibrary::CalculateDirection(Proprietor->GetVelocity(), Proprietor->GetActorRotation());
		LastVelocity = CurrentVelocity;
	}
}

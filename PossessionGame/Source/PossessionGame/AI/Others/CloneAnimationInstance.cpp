#include "CloneAnimationInstance.h"

void UCloneAnimationInstance::SetMovementDirectionForward(const bool bState)
{
	bIsDirectionForward = bState;
	MovementDirectionPlayRate = bState ? 1.f : -1.f;
}

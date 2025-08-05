#include "CustomCharacterMovementComponent.h"

bool UCustomCharacterMovementComponent::CanAttemptJump() const { return IsJumpAllowed() && (IsMovingOnGround() || IsFalling()); }

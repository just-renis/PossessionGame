#include "SpyHandsAnimInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

void USpyHandsAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	//GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->OnInvisibilityChange.AddUObject(this, &USpyHandsAnimInstance::SetIsInvisible); SOMETHING
}

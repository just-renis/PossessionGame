#include "RangerShield.h"
#include "Kismet/KismetSystemLibrary.h"

void ARangerShield::SetIsInUse(bool bInIsPlayerObject, const FVector& Location, const FRotator& Rotation)
{
	ShieldMoveTimes = 0;
	Super::SetIsInUse(bInIsPlayerObject, Location, Rotation);
	UKismetSystemLibrary::K2_SetTimer(this, "MoveShield", 0.05f, true);
}

void ARangerShield::MoveShield()
{
	if (++ShieldMoveTimes >= 10) UKismetSystemLibrary::K2_ClearTimer(this, "MoveShield");
	SetActorLocation(GetActorLocation() + Proprietor->GetActorForwardVector() * 10.f);
}

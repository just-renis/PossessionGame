#pragma once

#include "CoreMinimal.h"
#include "Destructible.h"
#include "RangerShield.generated.h"

UCLASS()
class POSSESSIONGAME_API ARangerShield : public ADestructible
{
	GENERATED_BODY()
public:
	void SetProprietor(const TObjectPtr<AActor>& InProprietor) { Proprietor = InProprietor; }
private:
	int32 ShieldMoveTimes;
	UPROPERTY()
	TObjectPtr<AActor> Proprietor;

	virtual void SetIsInUse(bool bInIsPlayerObject, const FVector& Location, const FRotator& Rotation) override;
	UFUNCTION()
	void MoveShield();
};

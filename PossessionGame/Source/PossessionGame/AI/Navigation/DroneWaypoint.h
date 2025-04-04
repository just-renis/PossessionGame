#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DroneWaypoint.generated.h"

UCLASS()
class POSSESSIONGAME_API ADroneWaypoint : public AActor
{
	GENERATED_BODY()
public:
	int32 GetOrder() const { return Order; }
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 Order = 0;
};

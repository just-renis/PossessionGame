#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PossessionGame/Enums/EGameState.h"
#include "DroneWaypoint.generated.h"

UCLASS()
class POSSESSIONGAME_API ADroneWaypoint : public AActor
{
	GENERATED_BODY()
public:
	bool GetIsAssociatedWithSecretRoom() const { return bIsAssociatedWithSecretRoom; }
	int32 GetOrder() const { return Order; }
	EGameState GetGameState() const { return GameState; }
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsAssociatedWithSecretRoom = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 Order = 0;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	EGameState GameState;
};

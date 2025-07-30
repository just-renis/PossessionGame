#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TutorialMonitorComponent.generated.h"

class AEnemy;

UCLASS()
class POSSESSIONGAME_API UTutorialMonitorComponent : public UActorComponent
{
	GENERATED_BODY()
public:
    void Init(const TObjectPtr<AEnemy>& InPlayer);
private:
	TObjectPtr<AEnemy> Player;
};

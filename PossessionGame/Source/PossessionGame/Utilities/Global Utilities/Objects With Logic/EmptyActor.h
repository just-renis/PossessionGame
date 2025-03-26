#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EmptyActor.generated.h"

UCLASS()
class POSSESSIONGAME_API AEmptyActor : public AActor
{
	GENERATED_BODY()

	AEmptyActor() { RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")); }
};

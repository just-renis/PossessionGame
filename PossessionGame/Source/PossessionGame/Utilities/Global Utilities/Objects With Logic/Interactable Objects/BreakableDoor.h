#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BreakableDoor.generated.h"

UCLASS()
class POSSESSIONGAME_API ABreakableDoor : public AActor
{
	GENERATED_BODY()

	ABreakableDoor();

	const bool bIsBreakable = false;
	UPROPERTY(EditDefaultsOnly, Category = "Mesh", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
};

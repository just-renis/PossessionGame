#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArtifactSpawnLocation.generated.h"

UCLASS()
class POSSESSIONGAME_API AArtifactSpawnLocation : public AActor
{
	GENERATED_BODY()
public:
	void SpawnArtifact() const;
};

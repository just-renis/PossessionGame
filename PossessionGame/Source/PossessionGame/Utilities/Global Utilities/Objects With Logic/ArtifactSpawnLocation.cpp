#include "ArtifactSpawnLocation.h"
#include "PoolObject.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"

void AArtifactSpawnLocation::SpawnArtifact() const
{
	GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::Artifact, GetActorLocation(), GetActorRotation())->Init();
}

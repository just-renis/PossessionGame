#include "SpawnLocation.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"

TObjectPtr<AEnemy> ASpawnLocation::SpawnEnemy(TSubclassOf<AEnemy> Enemy)
{
	return GetWorld()->SpawnActor<AEnemy>(Enemy, GetActorLocation(), GetActorRotation());
}
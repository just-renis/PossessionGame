#include "PrefabPlaceholder.h"
#include "PossessionGameGameModeBase.h"
#include "Prefab.h"

void APrefabPlaceholder::BeginPlay()
{
	Super::BeginPlay();
	TArray<TSubclassOf<APrefab>>* Prefabs = Cast<APossessionGameGameModeBase>(GetWorld()->GetAuthGameMode())->GetPrefabsByCategory(PrefabCategory);
	if (Prefabs != nullptr && Prefabs->Num() > 0)
	{
		const TSubclassOf<APrefab> PrefabToSpawn = (*Prefabs)[FMath::RandRange(0, Prefabs->Num() - 1)];
		if (PrefabToSpawn) GetWorld()->SpawnActor<APrefab>(PrefabToSpawn, GetActorLocation(), GetActorRotation());
	}
	Destroy();
}

#if WITH_EDITOR
void APrefabPlaceholder::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	DestroyTempPrefab();
	if (bCheckPrefab && PrefabToShowcase)
	{
		TempSpawnedPrefab = GetWorld()->SpawnActor<APrefab>(PrefabToShowcase, GetActorLocation(), GetActorRotation());
		if (TempSpawnedPrefab) TempSpawnedPrefab->SetFlags(RF_Transient);
	}
}
#endif

void APrefabPlaceholder::DestroyTempPrefab()
{
	if (TempSpawnedPrefab)
	{
		TempSpawnedPrefab->Destroy();
		TempSpawnedPrefab = nullptr;
	}
}
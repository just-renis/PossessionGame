#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PrefabPlaceholder.generated.h"

class APrefab;
enum class EPrefabCategory : uint8;

UCLASS()
class POSSESSIONGAME_API APrefabPlaceholder : public AActor
{
	GENERATED_BODY()
public:
	EPrefabCategory GetCategory() { return PrefabCategory; }
private:
	UPROPERTY(EditAnywhere, Category = "Category", meta = (AllowPrivateAccess = true))
	EPrefabCategory PrefabCategory;
	UPROPERTY(EditAnywhere, Category = "Category", meta = (AllowPrivateAccess = true))
	bool bCheckPrefab = false;
	UPROPERTY(EditAnywhere, Category = "Category", meta = (AllowPrivateAccess = true))
	TSubclassOf<APrefab> PrefabToShowcase;
	UPROPERTY()
	TObjectPtr<APrefab> TempSpawnedPrefab;

	virtual void BeginPlay() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	void DestroyTempPrefab();
};

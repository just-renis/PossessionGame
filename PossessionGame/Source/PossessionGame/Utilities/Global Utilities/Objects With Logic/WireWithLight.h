#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WireWithLight.generated.h"

UCLASS()
class POSSESSIONGAME_API AWireWithLight : public AActor
{
	GENERATED_BODY()

	AWireWithLight();

	UPROPERTY(EditAnywhere, Category = "Id", meta = (AllowPrivateAccess = true))
	int32 Id;
	UPROPERTY(EditDefaultsOnly, Category = "Mesh", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;

	virtual void BeginPlay() override;
	void ChangeColor(const int32 InId) const;
};

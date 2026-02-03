#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PyroBossFireFloor.generated.h"

UCLASS()
class POSSESSIONGAME_API APyroBossFireFloor : public AActor
{
	GENERATED_BODY()

	APyroBossFireFloor();

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 Id;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;

	virtual void BeginPlay() override;
	void TriggerFire(const TArray<int32>& InIds) const;
};

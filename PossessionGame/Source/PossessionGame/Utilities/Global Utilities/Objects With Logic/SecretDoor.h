#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SecretDoor.generated.h"

UCLASS()
class POSSESSIONGAME_API ASecretDoor : public AActor
{
	GENERATED_BODY()

	ASecretDoor();

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;

	virtual void BeginPlay() override;
	void DestroyThis();
};

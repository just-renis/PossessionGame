#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerminalCandle.generated.h"

UCLASS()
class POSSESSIONGAME_API ATerminalCandle : public AActor
{
	GENERATED_BODY()

	ATerminalCandle();

	UPROPERTY(EditAnywhere, Category = "Id", meta = (AllowPrivateAccess = true))
	int32 Id;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> BallMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;

	virtual void BeginPlay() override;
	void Interact(const int32 InId);
};

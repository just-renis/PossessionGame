#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerminalCandle.generated.h"

class ANiagaraActor;
class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API ATerminalCandle : public AActor
{
	GENERATED_BODY()

	ATerminalCandle();

	UPROPERTY(EditAnywhere, Category = "Id", meta = (AllowPrivateAccess = true))
	int32 Id;
	UPROPERTY(EditAnywhere, Category = "Vfx", meta = (AllowPrivateAccess = true))
	TObjectPtr<ANiagaraActor> EnergyWireVfx;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> BallMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> EnergyVfx;

	virtual void BeginPlay() override;
	void Interact(const int32 InId);
};

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerminalEnergyCore.generated.h"

UCLASS()
class POSSESSIONGAME_API ATerminalEnergyCore : public AActor
{
	GENERATED_BODY()
	
	ATerminalEnergyCore();

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;

	virtual void BeginPlay() override;
	void Interact(const int32 InId);
};

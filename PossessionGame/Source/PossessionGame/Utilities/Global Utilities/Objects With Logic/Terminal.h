#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Terminal.generated.h"

class UWidgetComponent;

UCLASS()
class POSSESSIONGAME_API ATerminal : public AActor
{
	GENERATED_BODY()

	ATerminal();

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> InteractableWidget;
};

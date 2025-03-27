#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "Marker.generated.h"

class UWidgetComponent;
class UObjectiveMarker;

UCLASS()
class POSSESSIONGAME_API AMarker : public APoolObject
{
	GENERATED_BODY()

	AMarker();
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> MarkerWidgetComponent;
	UPROPERTY()
	TObjectPtr<UObjectiveMarker> Marker;
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	virtual void BeginPlay() override;
	virtual void Init(bool bInIsPlayerObject) override;
	virtual void Disable(const bool bHide) override;
	UFUNCTION()
	void UpdateMarkerText() const;
};

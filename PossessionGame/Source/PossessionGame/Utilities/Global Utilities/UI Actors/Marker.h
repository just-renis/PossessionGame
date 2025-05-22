#pragma once

#include "CoreMinimal.h"
#include "Marker.generated.h"

class UWidgetComponent;
class UObjectiveMarker;

UCLASS()
class POSSESSIONGAME_API AMarker : public AActor
{
	GENERATED_BODY()
public:
	void Toggle(const bool bState);
	void DestroyThis();
private:
	AMarker();

	UPROPERTY()
	FTimerHandle MarkerUpdateHandle;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> MarkerWidgetComponent;
	UPROPERTY()
	TObjectPtr<UObjectiveMarker> Marker;
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	virtual void BeginPlay() override;
	UFUNCTION()
	void UpdateMarkerText() const;
};

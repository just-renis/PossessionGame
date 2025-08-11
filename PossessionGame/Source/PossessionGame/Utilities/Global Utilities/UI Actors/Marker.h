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
	void DestroyThis();
	void SetImage(const TObjectPtr<UTexture2D>& InMarkerTexture) const;
private:
	AMarker();

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> MarkerWidgetComponent;
	UPROPERTY()
	TObjectPtr<UObjectiveMarker> Marker;
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;
	UPROPERTY()
	FTimerHandle MarkerUpdateHandle;

	virtual void BeginPlay() override;
	void UpdateMarker() const;
};

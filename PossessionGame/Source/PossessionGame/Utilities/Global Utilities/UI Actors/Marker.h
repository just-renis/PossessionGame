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
	void SetImage(const TObjectPtr<UTexture2D>& InMarkerTexture) const;
	void SaveMarker(const int32 InId, const bool bSaveMarker);
private:
	AMarker();

	int32 Id;
	FTimerHandle MarkerUpdateHandle;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> MarkerWidgetComponent;
	UPROPERTY()
	TObjectPtr<UObjectiveMarker> Marker;
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	virtual void BeginPlay() override;
	void UpdateMarker() const;
	void DestroyThis();
	void DestroyThis(const int32 InId);
};

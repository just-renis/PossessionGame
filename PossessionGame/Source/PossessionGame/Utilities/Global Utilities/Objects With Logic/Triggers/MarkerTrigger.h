#pragma once

#include "CoreMinimal.h"
#include "MarkerTrigger.generated.h"

class USphereComponent;

UCLASS()
class POSSESSIONGAME_API AMarkerTrigger : public AActor
{
	GENERATED_BODY()
public:
	int32 GetMarkerTriggerIndex() const { return MarkerTriggerIndex; }
private:
	AMarkerTrigger();

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 NextMarkerToTrigger = -1;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 MarkerTriggerIndex = -1;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> Collision;

	virtual void BeginPlay() override;
	void OnOtherMarkerTriggerPassed(const int32 InNextMarkerToTrigger) const;
	UFUNCTION()
	void Initiate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

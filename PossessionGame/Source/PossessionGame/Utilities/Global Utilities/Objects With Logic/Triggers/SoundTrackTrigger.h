#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SoundTrackTrigger.generated.h"

enum class ESound : uint16;
class UBoxComponent;

UCLASS()
class POSSESSIONGAME_API ASoundTrackTrigger : public AActor
{
	GENERATED_BODY()

	ASoundTrackTrigger();

	UPROPERTY(EditAnywhere, Category = "Sound Track Data", meta = (AllowPrivateAccess = true))
	ESound SoundTrackName;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBoxComponent> Collision;

	virtual void BeginPlay() override;
	UFUNCTION()
	void Initiate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

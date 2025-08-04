#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HintTrigger.generated.h"

class UBoxComponent;

UCLASS()
class POSSESSIONGAME_API AHintTrigger : public AActor
{
	GENERATED_BODY()

	AHintTrigger();

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBoxComponent> Collision;
	
	virtual void BeginPlay() override;
	UFUNCTION()
	void Initiate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

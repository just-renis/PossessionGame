#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TeleportTrigger.generated.h"

class USphereComponent;

UCLASS()
class POSSESSIONGAME_API ATeleportTrigger : public AActor
{
	GENERATED_BODY()

	ATeleportTrigger();
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 Id;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> Collision;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void TeleportPlayer(int32 InId, const TObjectPtr<AActor>& InPlayer);
	UFUNCTION()
	void Initiate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

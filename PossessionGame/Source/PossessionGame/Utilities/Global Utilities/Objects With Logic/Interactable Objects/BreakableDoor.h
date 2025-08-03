#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BreakableDoor.generated.h"

class USphereComponent;

UCLASS()
class POSSESSIONGAME_API ABreakableDoor : public AActor
{
	GENERATED_BODY()

	ABreakableDoor();

	bool bIsBreakable = false;
	UPROPERTY(EditAnywhere, Category = "Individual Values", meta = (AllowPrivateAccess = true))
	float Force = 50.f;
	UPROPERTY(EditDefaultsOnly, Category = "Mesh", meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> Collision;
	UPROPERTY(EditDefaultsOnly, Category = "Mesh", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;

	virtual void BeginPlay() override;
	void EnableCollision() const;
	void TryBreakingDoor() const;
	UFUNCTION()
	virtual void EnableBreakable(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void DisableBreakable(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};

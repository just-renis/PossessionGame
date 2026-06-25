#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WallLaser.generated.h"

class AEnemy;
class UCapsuleComponent;

UCLASS()
class POSSESSIONGAME_API AWallLaser : public AActor
{
	GENERATED_BODY()

	AWallLaser();

	FTimerHandle DamageOvertimeHandle;
	UPROPERTY(EditAnywhere, Category = "Group", meta = (AllowPrivateAccess = true))
	int32 Group;
	UPROPERTY(EditAnywhere, Category = "Group", meta = (AllowPrivateAccess = true))
	int32 Id;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float DamageTick = 0.2f;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float Damage = 5.f;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCapsuleComponent> DamageRadius;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_Material;
	UPROPERTY()
	TObjectPtr<AEnemy> Target;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void EnableLaser(const TArray<int32>& InIds, const int32 InGroup);
	void DealDamage();
	void AddTarget(const TObjectPtr<AEnemy>& InTarget);
	void RemoveTarget();
	UFUNCTION()
	void AddOverlappedTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void RemoveOverlappedTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};

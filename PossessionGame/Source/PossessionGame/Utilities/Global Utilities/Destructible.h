#pragma once

#include "CoreMinimal.h"
#include "PoolObject.h"
#include "GameFramework/Actor.h"
#include "Destructible.generated.h"

class UDestructibleData;

UCLASS()
class POSSESSIONGAME_API ADestructible : public APoolObject
{
	GENERATED_BODY()
public:
	ADestructible();
	
	bool GetIsDeactivated() { return bIsDeactivated; }
	
	void HandleDeactivation();
private:
	bool bIsDeactivated = false;
	bool bIsAffectedByPhysics = false;
	bool bIsPlayerObject;
	float LifeSpan;
	float Hp;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UDestructibleData> Data;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;

	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void SetIsInUse(bool bInIsPlayerObject, const FVector& Location, const FRotator& Rotation) override;
	UFUNCTION()
	void DestroyThis();
};

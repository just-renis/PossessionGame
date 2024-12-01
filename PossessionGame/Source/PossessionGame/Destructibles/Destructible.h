#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PossessionGame/Utilities/Global Utilities/PoolObject.h"
#include "Destructible.generated.h"

class UDestructibleData;

UCLASS()
class POSSESSIONGAME_API ADestructible : public APoolObject
{
	GENERATED_BODY()
public:
	ADestructible();
	
	bool GetIsDeactivated() const { return bIsDeactivated; }
	
	void HandleDeactivation();
	virtual void SetIsInUse(bool bInIsPlayerObject, const FVector& Location, const FRotator& Rotation) override;
protected:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void DestroyThis();
private:
	bool bIsDeactivated = false;
	bool bIsAffectedByPhysics = false;
	bool bIsPlayerObject;
	float LifeSpan;
	float Hp;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UDestructibleData> Data;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};

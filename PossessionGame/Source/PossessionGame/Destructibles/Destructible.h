#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
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
	virtual void Init(bool bInIsPlayerObject) override;
protected:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void DestroyThis();
private:
	bool bIsDeactivated = false;
	bool bIsAffectedByPhysics = false;
	bool bIsPlayerObject;
	float LifeSpan;
	float Hp;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UDestructibleData> Data;
	UPROPERTY()
	FTimerHandle DestroyHandle;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};

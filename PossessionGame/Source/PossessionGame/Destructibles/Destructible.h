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
	bool GetIsPlayerObject() const { return bIsPlayerObject; }
	TObjectPtr<UStaticMeshComponent> GetMesh() { return Mesh; }
	
	void HandleDeactivation();
	virtual void Init(bool bInIsPlayerObject) override;
	virtual void InitWithProprietor(const TObjectPtr<AEnemy> InProprietor) override;
	void ReceiveDamage(const float InDamage);
	virtual void DestroyThis();
protected:
	bool bIsPlayerObject;
	float LifeSpan;
	float Hp;
	float DamageReduction;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UDestructibleData> Data;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetThisLifeSpan();
	virtual void SubtractHp(const float InDamage);
	virtual void SetUpgradesApplication();
private:
	bool bIsDeactivated = false;
	bool bIsAffectedByPhysics = false;
	FTimerHandle DestroyHandle;
};

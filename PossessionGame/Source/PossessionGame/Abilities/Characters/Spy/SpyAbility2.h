#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "SpyAbility2.generated.h"

class ASpy;

UCLASS()
class POSSESSIONGAME_API USpyAbility2 : public UAbility
{
	GENERATED_BODY()
public:
	virtual void AI_Execute() override;
	
	virtual void P_Execute() override;
private:
	float TransferSpeed = 150.f;
	FTimerHandle TransferHandle;
	FVector TargetLocation;
	UPROPERTY()
	TObjectPtr<AEnemy> Target;
	UPROPERTY()
	TObjectPtr<ASpy> Proprietor;
	
	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) override;
	void CastLineTrace();
	void TryHitEnemy(const FHitResult& HitResult);
	void InitiateProcess();
	void ProcessTransfer();
	void FinishTransfer();
};
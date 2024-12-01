#pragma once

#include "CoreMinimal.h"
#include "Destructible.h"
#include "RangerShield.generated.h"

UCLASS()
class POSSESSIONGAME_API ARangerShield : public ADestructible
{
	GENERATED_BODY()
public:
	void SetProprietor(const TObjectPtr<AActor>& InProprietor) { Proprietor = InProprietor; }
private:
	int32 ShieldMaterialValueChanger;
	UPROPERTY()
	TObjectPtr<AActor> Proprietor;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> M_ForceField;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> M_Shield;

	virtual void BeginPlay() override;
	virtual void SetIsInUse(bool bInIsPlayerObject, const FVector& Location, const FRotator& Rotation) override;
	virtual void DestroyThis() override;
	void UpdateShieldMaterialValues(const float InOpacity, const float InProgress) const;
	UFUNCTION()
	void MoveShield();
	UFUNCTION()
	void DissolveShield();
};

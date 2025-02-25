#pragma once

#include "CoreMinimal.h"
#include "PoolObject.h"
#include "DamageNumber.generated.h"

class UWidgetComponent;
class UDamageNumberUI;

UCLASS()
class POSSESSIONGAME_API ADamageNumber : public APoolObject
{
	GENERATED_BODY()

	ADamageNumber();

	virtual void BeginPlay() override;
	virtual void Init(const float InData) override;
	UFUNCTION()
	void ReturnToPool();

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> DamageNumberWidgetComponent;
	UPROPERTY()
	TObjectPtr<UDamageNumberUI> DamageNumber;
};

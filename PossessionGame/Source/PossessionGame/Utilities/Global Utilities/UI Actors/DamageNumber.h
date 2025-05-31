#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "DamageNumber.generated.h"

class UWidgetComponent;
class UDamageNumberUI;

UCLASS()
class POSSESSIONGAME_API ADamageNumber : public APoolObject
{
	GENERATED_BODY()

	ADamageNumber();
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Init(const float InData, const bool bHide) override;
	virtual void Disable(const bool bHide) override;
	void ReturnToPool();

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> DamageNumberWidgetComponent;
	UPROPERTY()
	TObjectPtr<UDamageNumberUI> DamageNumber;
	UPROPERTY()
	FTimerHandle ReturnToPoolHandle;
};

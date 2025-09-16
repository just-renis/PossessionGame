#pragma once

#include "CoreMinimal.h"
#include "Throwable.h"
#include "RangerMine.generated.h"

class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API ARangerMine : public AThrowable
{
	GENERATED_BODY()

	bool bAlreadyLanded = false;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> MineRadiusVfx;

	virtual void BeginPlay() override;
	virtual void OnImpact(const FHitResult& Hit) override;
};

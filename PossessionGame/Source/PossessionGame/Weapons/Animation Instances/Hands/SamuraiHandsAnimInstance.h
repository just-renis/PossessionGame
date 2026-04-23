#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "SamuraiHandsAnimInstance.generated.h"

class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API USamuraiHandsAnimInstance : public UHandsAnimInstance
{
	GENERATED_BODY()
public:
	void PlaySlashVfx();
private:
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> SlashVfx;
};

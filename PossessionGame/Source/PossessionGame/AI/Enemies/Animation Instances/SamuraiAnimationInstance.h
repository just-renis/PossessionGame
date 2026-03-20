#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "SamuraiAnimationInstance.generated.h"

enum class EVfx : uint16;
class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API USamuraiAnimationInstance : public UEnemyAnimationInstance
{
	GENERATED_BODY()
public:
	void PlaySlashVfx(const EVfx InVfxName, const int32 SlashIndex);
private:
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> FirstSlashVfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> SecondSlashVfx;

	virtual void OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted) override;
};

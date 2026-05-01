#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "SamuraiHandsAnimInstance.generated.h"

class ASamuraiWeapon;
class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API USamuraiHandsAnimInstance : public UHandsAnimInstance
{
	GENERATED_BODY()
public:
	void PlaySlashVfx(const bool bIsFirstSlash);
	void SetDeflectState(const bool bState) { bIsDeflectEnabled = bState; }
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bIsDeflectEnabled = false;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> FirstSlashVfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> SecondSlashVfx;

	virtual void OnMontageEnd(UAnimMontage* InMontage, const bool bIsInterrupted) override;
};

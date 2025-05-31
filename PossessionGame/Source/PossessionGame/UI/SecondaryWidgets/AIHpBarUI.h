#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AIHpBarUI.generated.h"

class UProgressBar;

UCLASS()
class POSSESSIONGAME_API UAIHpBarUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateHpBar(const float InCurrentHp, const float InMaxHp);
private:
	float StepHpPercentage;
	UPROPERTY(meta = (BindWIdget))
	TObjectPtr<UProgressBar> HpBar;
	UPROPERTY(meta = (BindWIdget))
	TObjectPtr<UProgressBar> HpWhiteBar;
	UPROPERTY()
	FTimerHandle UpdateWhiteHpBarHandle;

	UFUNCTION()
	void UpdateWhiteHpBar();
};

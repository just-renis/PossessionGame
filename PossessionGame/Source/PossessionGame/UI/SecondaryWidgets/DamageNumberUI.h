#pragma once

#include "CoreMinimal.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/UserWidget.h"
#include "DamageNumberUI.generated.h"

class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UDamageNumberUI : public UUserWidget
{
	GENERATED_BODY()
public:
	float GetAnimationLength() const { return DamageNumberAnim1->GetEndTime(); }
	
	void SetDamageAmount(float InDamageAmount);
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DamageNumberText;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> DamageNumberAnim1;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> DamageNumberAnim2;
};

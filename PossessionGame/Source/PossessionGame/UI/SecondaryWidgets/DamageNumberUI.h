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
	float GetAnimationLength() const { return DamageNumberAnimation->GetEndTime(); }
	
	void SetDamageAmount(const float InDamageAmount);
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DamageNumberText;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> DamageNumberAnimation;
};

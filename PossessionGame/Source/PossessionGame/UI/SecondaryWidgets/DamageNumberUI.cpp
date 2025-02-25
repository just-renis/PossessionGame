#include "DamageNumberUI.h"
#include "Components/TextBlock.h"

void UDamageNumberUI::SetDamageAmount(const float InDamageAmount)
{
	DamageNumberText->SetText(FText::AsNumber(InDamageAmount));
	PlayAnimation(DamageNumberAnimation);
}

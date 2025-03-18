#include "DamageNumberUI.h"
#include "Components/TextBlock.h"

void UDamageNumberUI::SetDamageAmount(const float InDamageAmount)
{
	if (DamageNumberText) DamageNumberText->SetText(FText::AsNumber(static_cast<int32>(InDamageAmount)));
	if (DamageNumberAnimation) PlayAnimation(DamageNumberAnimation);
}

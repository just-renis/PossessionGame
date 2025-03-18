#include "DamageNumberUI.h"
#include "Components/TextBlock.h"

void UDamageNumberUI::SetDamageAmount(const float InDamageAmount)
{
	DamageNumberText->SetText(FText::AsNumber(static_cast<int32>(InDamageAmount)));
	PlayAnimation(DamageNumberAnimation);
}

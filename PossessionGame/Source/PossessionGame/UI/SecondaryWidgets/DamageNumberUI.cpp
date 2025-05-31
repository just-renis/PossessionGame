#include "DamageNumberUI.h"
#include "Components/TextBlock.h"

void UDamageNumberUI::SetDamageAmount(const float InDamageAmount)
{
	if (DamageNumberText) DamageNumberText->SetText(FText::AsNumber(static_cast<int32>(InDamageAmount)));
	DamageNumberText->SetRenderTransform(FWidgetTransform());
	PlayAnimation(FMath::RandBool() ? DamageNumberAnim1 : DamageNumberAnim2);
}

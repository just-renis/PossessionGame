#include "DamageNumberUI.h"
#include "Components/TextBlock.h"

void UDamageNumberUI::SetDamageAmount(const float InDamageAmount)
{
	if (DamageNumberText) DamageNumberText->SetText(FText::AsNumber(static_cast<int32>(InDamageAmount)));
	if (!DamageNumberAnim1 || !DamageNumberAnim2) return;
	DamageNumberText->SetRenderTransform(FWidgetTransform());
	if (FMath::RandBool()) PlayAnimation(DamageNumberAnim1);
	else PlayAnimation(DamageNumberAnim2);
}

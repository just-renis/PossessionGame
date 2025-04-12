#include "DamageNumberUI.h"
#include "Components/TextBlock.h"

void UDamageNumberUI::SetDamageAmount(const float InDamageAmount)
{
	if (DamageNumberText) DamageNumberText->SetText(FText::AsNumber(static_cast<int32>(InDamageAmount)));
	if (!DamageNumberAnim1 || !DamageNumberAnim2) return;
	if (FMath::RandBool())
	{
		UE_LOG(LogTemp, Warning, TEXT("PLAY ANIMATION 1"));
		PlayAnimation(DamageNumberAnim1);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PLAY ANIMATION 2"));
		PlayAnimation(DamageNumberAnim2);
	}
}

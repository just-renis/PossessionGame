#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilityIconUI.generated.h"

class UButton;
class UImage;

UCLASS()
class POSSESSIONGAME_API UAbilityIconUI : public UUserWidget
{
	GENERATED_BODY()
public:
	bool GetIsInSlot() const { return bIsInSlot; }
	TObjectPtr<UButton> GetButton() { return AbilityButton; }
	
	void SetAbilityData(const TObjectPtr<UTexture2D>& InIcon, const FText& InDetailsText);
	void PlayWobbleAnim() { PlayAnimation(WobbleAnim); }
	void SetIsInSlot(const bool bState);
private:
	bool bIsInSlot = false;
	FText DetailsText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AbilityButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> AbilityIcon;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> WobbleAnim;

	virtual void NativeConstruct() override;
	UFUNCTION()
	void OnHovered();
	UFUNCTION()
	void OnUnHovered();
};

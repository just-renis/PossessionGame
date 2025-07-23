#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PossessionGame/Utilities/Data Utilities/FAbilityIconData.h"
#include "AbilityIconUI.generated.h"

enum class ECharacterType : uint8;
enum class EAbility : uint8;
class UButton;
class UImage;

UCLASS()
class POSSESSIONGAME_API UAbilityIconUI : public UUserWidget
{
	GENERATED_BODY()
public:
	bool GetIsInSlot() const { return bIsInSlot; }
	bool GetIsLocked() const { return bIsLocked; }
	int32 GetAbilityIndex() const { return AbilityIndex; }
	TObjectPtr<UButton> GetButton() { return AbilityButton; }
	
	void PlayWobbleAnim() { PlayAnimation(WobbleAnim); }
	void SetIsInSlot(const bool bState);
	void SetIsLocked(const bool bState) { bIsLocked = bState; }
	void SetData(const ECharacterType InCharacter, const FAbilityIconData& InData);
private:
	bool bIsInSlot = false;
	bool bIsLocked = true;
	int32 AbilityIndex;
	FText DetailsText;
	UPROPERTY(EditDefaultsOnly, Category = "Ability Data", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> LockedTexture;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> AbilityIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AbilityButton;
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

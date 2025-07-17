#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SecretBoostCardUI.generated.h"

DECLARE_DELEGATE_TwoParams(FShowDetailsPanelContents, const bool, const FText&);

enum class EVariation : uint8;
class UImage;
class UButton;

UCLASS()
class POSSESSIONGAME_API USecretBoostCardUI : public UUserWidget
{
	GENERATED_BODY()
public:
	FShowDetailsPanelContents ShowDetailsPanelContents;
	
	void DisableButton() const;
private:
	UPROPERTY(EditAnywhere, Category = "Boost", meta = (AllowPrivateAccess = true))
	int32 BoostValue;
	UPROPERTY(EditAnywhere, Category = "Boost", meta = (AllowPrivateAccess = true))
	EVariation BoostType;
	UPROPERTY(EditAnywhere, Category = "Texture", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> BoostTexture;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CardIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BoostButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> UnHoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> SelectAnim;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	UFUNCTION()
	void OnHovered();
	UFUNCTION()
	void OnUnHovered();
	UFUNCTION()
	void SaveChosenBoost();
};

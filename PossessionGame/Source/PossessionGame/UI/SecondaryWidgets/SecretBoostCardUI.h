#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SecretBoostCardUI.generated.h"

class UTextBlock;
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
	bool bChosen = false;
	UPROPERTY(EditAnywhere, Category = "Boost", meta = (AllowPrivateAccess = true))
	FText BoostText;
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
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SecretBoostText;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> UnHoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> SelectAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> NotSelectAnim;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	void PlayNotSelectedAnim();
	UFUNCTION()
	void OnHovered();
	UFUNCTION()
	void OnUnHovered();
	UFUNCTION()
	void SaveChosenBoost();
};

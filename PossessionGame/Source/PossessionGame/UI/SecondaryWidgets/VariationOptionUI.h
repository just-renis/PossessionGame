#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VariationOptionUI.generated.h"

class UImage;
class UTextBlock;
class UBorder;

DECLARE_DELEGATE_TwoParams(FShowDetailsPanel, const bool, const FText&);

UCLASS()
class POSSESSIONGAME_API UVariationOptionUI : public UUserWidget
{
	GENERATED_BODY()
public:
	FShowDetailsPanel ShowDetailsPanel;
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsLocked = false;
	UPROPERTY(EditAnywhere, Category = "Text", meta = (AllowPrivateAccess = true))
	FText Text;
	UPROPERTY(EditAnywhere, Category = "Text", meta = (AllowPrivateAccess = true))
	FText DetailsPanelText;
	UPROPERTY(EditAnywhere, Category = "Icon", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> VariationOptionIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> VariationOptionBorder;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> LockedImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> OptionText;

	virtual void NativePreConstruct() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
};

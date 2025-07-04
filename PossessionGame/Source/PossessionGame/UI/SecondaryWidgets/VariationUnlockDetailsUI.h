#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PossessionGame/Enums/EVariation.h"
#include "VariationUnlockDetailsUI.generated.h"

class USaveSlot;
class UHorizontalBox;
class UMainMenuButtonUI;
class UCheckBox;
class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UVariationUnlockDetailsUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetDetailsText(const FText& InText) const;
	void SetTaskCompleted(const bool bState) const;
	void SetUnlockedVariation(const bool bState) const;
	void SetupDetailsPanel(const EVariation InVariation, const int32 InUnlockPrice);
	void SetSaveSlot(const TObjectPtr<USaveSlot>& InSaveSlot) { SaveSlot = InSaveSlot; }
private:
	int32 UnlockPrice = INT_MAX;
	UPROPERTY(EditAnywhere, Category = "Details Panel", meta = (AllowPrivateAccess = true))
	bool bIsCheckBoxNeeded = false;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCheckBox> DetailsCheck;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DetailsText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CostPriceText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> UnlockButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> CostPriceBox;
	UPROPERTY()
	EVariation Variation = EVariation::Default;
	UPROPERTY()
	TObjectPtr<USaveSlot> SaveSlot;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	void ChangeVisibilityOfUnlockElements(const bool bState) const;
	UFUNCTION()
	void UnlockVariation();
};

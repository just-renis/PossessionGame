#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VariationCurrencyPanelUI.generated.h"

enum class EVariation : uint8;
class USaveSlot;
class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UVariationCurrencyPanelUI : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrencyText;
	UPROPERTY()
	TObjectPtr<USaveSlot> PlayerSaveSlot;
	
	virtual void NativeConstruct() override;
	void AddCurrency(const int32 InCurrencyAmount) const;
	void UpdateCurrencyText() const;
	void UpdateCurrencyText(const EVariation InVariationName) const;
};

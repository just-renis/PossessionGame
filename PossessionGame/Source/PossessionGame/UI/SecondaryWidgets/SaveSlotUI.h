#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveSlotUI.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class POSSESSIONGAME_API USaveSlotUI : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Save Slots Names", meta = (AllowPrivateAccess = true))
	FString SlotName;
	UPROPERTY(EditAnywhere, Category = "Save Slots Names", meta = (AllowPrivateAccess = true))
	FString RunSlotName;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SaveSlotButton;
	UPROPERTY(meta = (BindWidget, MultiLine))
	TObjectPtr<UTextBlock> SaveSlotText;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> SelectAnim;

	virtual void NativeConstruct() override;
	void SetText() const;
	UFUNCTION()
	void OnClicked();
	UFUNCTION()
	void OnHovered();
	UFUNCTION()
	void OnUnHovered();
};

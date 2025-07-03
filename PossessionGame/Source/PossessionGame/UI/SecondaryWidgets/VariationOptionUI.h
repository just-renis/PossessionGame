#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VariationOptionUI.generated.h"

class USaveSlot;
enum class EVariation : uint8;
enum class ECharacterType : uint8;
class UButton;
class UImage;
class UTextBlock;
class UBorder;

DECLARE_DELEGATE_FourParams(FShowDetailsPanel, const bool, const EVariation, const FString&, const FString&);

UCLASS()
class POSSESSIONGAME_API UVariationOptionUI : public UUserWidget
{
	GENERATED_BODY()
public:
	FShowDetailsPanel ShowDetailsPanel;

	bool GetIsLocked() const { return bIsLocked; }

	void UpdateUnlockProgressText(const TObjectPtr<USaveSlot>& InSaveSlot);
	void UnlockVariation();
	void SetSelected(const bool bState);
	void SetButtonState(const bool bState) const;
private:
	bool bIsSelected = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsLocked = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	EVariation Variation;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	ECharacterType CharacterType;
	UPROPERTY(EditAnywhere, Category = "Text", meta = (AllowPrivateAccess = true, MultiLine = true))
	FString VariationDetailsPanelText;
	UPROPERTY(EditAnywhere, Category = "Text", meta = (AllowPrivateAccess = true, MultiLine = true))
	FString DefaultUnlockText;
	UPROPERTY(EditAnywhere, Category = "Icon", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> VariationOptionIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> VariationOptionBorder;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> LockedImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> OptionButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SelectedText;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> SelectAnim;
	UPROPERTY()
	FString UnlockProgressText;

	FString GetUnlockProgressString(const TObjectPtr<USaveSlot>& InSaveSlot) const;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	void SelectionChange(const ECharacterType InCharacterType, const EVariation InVariationName);
	UFUNCTION()
	void SelectVariation();
	UFUNCTION()
	void VariationOptionHovered();
	UFUNCTION()
	void VariationOptionUnHovered();
};

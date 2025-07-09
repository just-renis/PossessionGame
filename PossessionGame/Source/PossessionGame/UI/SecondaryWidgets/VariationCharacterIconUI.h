#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VariationCharacterIconUI.generated.h"

class UButton;
class UBorder;

DECLARE_DELEGATE_OneParam(FOnVariationClicked, const int32);

UCLASS()
class POSSESSIONGAME_API UVariationCharacterIconUI : public UUserWidget
{
	GENERATED_BODY()
public:
	FOnVariationClicked OnVariationClicked;

	void SetEventsEnabled(const bool bState);
	UFUNCTION()
	void PlayHoverAnim();
private:
	bool bEventsEnabled = true;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 VariationIndex = 0;
	UPROPERTY(EditAnywhere, Category = "Icon", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> VariationCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> VariationCharacterBorder;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> VariationCharacterButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> SelectAnim;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	void PlaySelectedAnim();
	UFUNCTION()
	void HandleClicked();
	UFUNCTION()
	void PlayUnHoverAnim();
};

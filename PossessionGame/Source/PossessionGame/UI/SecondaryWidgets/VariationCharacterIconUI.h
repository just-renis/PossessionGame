#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VariationCharacterIconUI.generated.h"

enum class ECharacterType : uint8;
class UButton;
class UBorder;

DECLARE_DELEGATE_OneParam(FOnVariationClicked, const ECharacterType);

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
	ECharacterType CharacterName;
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
	UFUNCTION()
	void HandleClicked();
	UFUNCTION()
	void PlayUnHoverAnim();
};

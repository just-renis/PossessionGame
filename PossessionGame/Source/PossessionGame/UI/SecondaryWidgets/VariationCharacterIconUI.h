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
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 VariationIndex = 0;
	UPROPERTY(EditAnywhere, Category = "Icon", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> VariationCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> VariationCharacterBorder;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> VariationCharacterButton;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	UFUNCTION()
	void HandleClicked();
};

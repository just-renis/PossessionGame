#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VariationCharacterIconUI.generated.h"

class UBorder;

UCLASS()
class POSSESSIONGAME_API UVariationCharacterIconUI : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Icon", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> VariationCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> VariationCharacterBorder;

	virtual void NativePreConstruct() override;
};

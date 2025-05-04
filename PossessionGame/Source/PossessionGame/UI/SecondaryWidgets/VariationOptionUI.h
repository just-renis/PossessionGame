#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VariationOptionUI.generated.h"

class UBorder;

UCLASS()
class POSSESSIONGAME_API UVariationOptionUI : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Icon", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> VariationOptionIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> VariationOptionBorder;

	virtual void NativePreConstruct() override;
};

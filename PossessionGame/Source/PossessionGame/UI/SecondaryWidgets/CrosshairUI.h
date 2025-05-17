#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrosshairUI.generated.h"

class UImage;

UCLASS()
class POSSESSIONGAME_API UCrosshairUI : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> CrosshairTexture;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CrosshairImage;
	
	virtual void NativePreConstruct() override;
};

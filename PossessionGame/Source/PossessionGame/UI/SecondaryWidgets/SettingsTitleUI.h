#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsTitleUI.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class POSSESSIONGAME_API USettingsTitleUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetTitleText(FText InTitleText);
private:
	UPROPERTY(EditAnywhere, Category = "Title Settings", meta = (AllowPrivateAccess = true))
	int32 FontSize = 24;
	UPROPERTY(EditAnywhere, Category = "Title Settings", meta = (AllowPrivateAccess = true))
	FVector2D ImageSize = FVector2D(350, 15);
	UPROPERTY(EditAnywhere, Category = "Title Settings", meta = (AllowPrivateAccess = true))
	FText TitleText;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UImage> Decoration;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Title;

	virtual void NativePreConstruct() override;
};

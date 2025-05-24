#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsTitleUI.generated.h"

class UTextBlock;

UCLASS()
class POSSESSIONGAME_API USettingsTitleUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetTitleText(const FText& InTitleText) const;
private:
	UPROPERTY(EditAnywhere, Category = "Title Settings", meta = (AllowPrivateAccess = true))
	int32 FontSize = 20;
	UPROPERTY(EditAnywhere, Category = "Title Settings", meta = (AllowPrivateAccess = true))
	FText TitleText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Title;

	virtual void NativePreConstruct() override;
};

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpgradeDetailsPanelUI.generated.h"

class UUpgradeData;
class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UUpgradeDetailsPanelUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetUpgradeText(const FString& InText) const;
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UpgradeText;
};

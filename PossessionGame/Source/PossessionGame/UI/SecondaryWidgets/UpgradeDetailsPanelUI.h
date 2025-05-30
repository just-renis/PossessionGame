#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpgradeDetailsPanelUI.generated.h"

enum class EUpgradeAttribute : uint8;
struct FUpgradeAttributes;
class UUpgradeData;
class UTextBlock;
class UButton;

UCLASS()
class POSSESSIONGAME_API UUpgradeDetailsPanelUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void Setup(const TObjectPtr<UUpgradeData>& InUpgradeData);
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 ChoiceIndex;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UpgradeText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> UpgradeChoiceButton;
	UPROPERTY()
	TObjectPtr<UUpgradeData> UpgradeData;

	FString ConvertUpgradeAttributeValueToString(const float InValue);
	FString GetUpgradeTextFromSingularAttributeValue(const TPair<EUpgradeAttribute, float> InAttribute);

	virtual void NativeConstruct() override;
	void ProcessCurrentUpgradeData(FUpgradeAttributes& PreviousUpgradeData, FUpgradeAttributes& CurrentUpgradeData);
	void SetUpgradeText();
	UFUNCTION()
	void SelectUpgrade();
};

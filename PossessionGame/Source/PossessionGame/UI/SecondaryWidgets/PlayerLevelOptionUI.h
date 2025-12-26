#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerLevelOptionUI.generated.h"

class URichTextBlock;
class UButton;
enum class EPlayerLevelUpgradeId : uint8;

UCLASS()
class POSSESSIONGAME_API UPlayerLevelOptionUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void Setup(const bool bIsUnlocked, const TObjectPtr<UPlayerLevelOptionUI>& InOtherOptionWidget, const EPlayerLevelUpgradeId InId, const EPlayerLevelUpgradeId InOtherOptionId, const FText& InText, const bool bIsChosen);
private:
	bool bIsSelected = false;
	EPlayerLevelUpgradeId Id;
	EPlayerLevelUpgradeId OtherOptionId;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> OptionText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> OptionButton;
	UPROPERTY()
	TObjectPtr<UObject> NotSelectedTexture;

	virtual void NativeConstruct() override;
	void SetAsChosen() const;
	UFUNCTION()
	void OnSelected();
	UFUNCTION()
	void OnOtherOptionSelected();
	UFUNCTION()
	void OnHoverTrigger();
};

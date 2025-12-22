#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerLevelOptionUI.generated.h"

class UButton;
enum class EPlayerLevelUpgradeId : uint8;
class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UPlayerLevelOptionUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void Setup(const TObjectPtr<UPlayerLevelOptionUI>& InOtherOptionWidget, const EPlayerLevelUpgradeId InId, const EPlayerLevelUpgradeId InOtherOptionId, const FText& InText, const bool bIsChosen);
private:
	bool bIsSelected = false;
	EPlayerLevelUpgradeId Id;
	EPlayerLevelUpgradeId OtherOptionId;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> OptionText;
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
};

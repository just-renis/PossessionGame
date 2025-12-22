#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerLevelSystemUI.generated.h"

enum class EPlayerLevelUpgradeId : uint8;
class UVerticalBox;

UCLASS()
class POSSESSIONGAME_API UPlayerLevelSystemUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void OpenUI();
	void CloseUI();
private:
	int32 PlayerLevelMaxAmount = 20;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> PlayerLevelOptionClass;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TMap<EPlayerLevelUpgradeId, FText> UpgradeDescriptions;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> OptionsBox;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnim;
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeOutDelegate;

	virtual void NativeConstruct() override;
	void SetupPlayerOptions();
	void Toggle(const bool bState);
	UFUNCTION()
	void Hide();
};

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeathScreenUI.generated.h"

class UMainMenuButtonUI;

UCLASS()
class POSSESSIONGAME_API UDeathScreenUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void OpenUI();
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> RestartButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> ReturnToMainMenuButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeOutAnim;

	virtual void NativeConstruct() override;
	void Toggle(const bool bState);
	void ToggleButtons(const bool bState) const;
	UFUNCTION()
	void ReturnToMainMenu();
	UFUNCTION()
	void Restart();
};

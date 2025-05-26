#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuUI.generated.h"

class UMainMenuButtonUI;
class UTextBlock;
class UButton;

UCLASS()
class POSSESSIONGAME_API UPauseMenuUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void OpenUI();
	void ChangeUIToDeathMenu();
	UFUNCTION()
	void CloseUI();
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> ContinueButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> SettingsButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> ReturnToMenuButton;

	virtual void NativeConstruct() override;
	void Toggle(bool bState);
	void SetupButtons();
	UFUNCTION()
	void OpenSettings();
	UFUNCTION()
	void ReturnToMainMenu();
	UFUNCTION()
	void RestartLevel();
	UFUNCTION()
	void QuitGame();
};

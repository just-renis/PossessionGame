#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuUI.generated.h"

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
	TObjectPtr<UTextBlock> ContinueText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SettingsText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ReturnToMainMenuText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ContinueButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SettingsButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ReturnToMainMenuButton;

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

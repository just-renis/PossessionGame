#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuUI.generated.h"

class UButton;

UCLASS()
class POSSESSIONGAME_API UPauseMenuUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void OpenUI();
	UFUNCTION()
	void CloseUI();
private:
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> ContinueButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> SettingsButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> ReturnToMainMenuButton;

	virtual void NativeConstruct() override;
	void Toggle(bool bState);
	void SetupButtons();
	UFUNCTION()
	void OpenSettings();
	UFUNCTION()
	void ReturnToMainMenu();
};

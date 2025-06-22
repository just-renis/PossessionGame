#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeathMenuUI.generated.h"

class UMainMenuButtonUI;

UCLASS()
class POSSESSIONGAME_API UDeathMenuUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void OpenUI();
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> RestartButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> ReturnToMenuButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeOutAnim;

	virtual void NativeConstruct() override;
	void SetupButtons();
	UFUNCTION()
	void ReturnToMainMenu();
	UFUNCTION()
	void Restart();
};

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveSlotMenuUI.generated.h"

class UConfirmationPopupUI;

UCLASS()
class POSSESSIONGAME_API USaveSlotMenuUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void OpenUI();
	void CloseUI();
private:
	bool bIsConfirmationPanelOpen = false;
	FString SaveSlotName;
	FString RunSaveSlotName;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UConfirmationPopupUI> ConfirmationPopup;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnim;
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeInAnimFinishedDelegate;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	void Toggle(const bool bState);
	void OpenConfirmationPopup(const FString& InSlotName, const FString& InRunSlotName);
	void ResetToDefaultPlayerInputMode() const;
	UFUNCTION()
	void OpenCurrentSaveLevel();
	UFUNCTION()
	void OverwriteSaveAndOpenLevel();
	UFUNCTION()
	void LoadSaveAndOpenLevel();
	UFUNCTION()
	void GoBack();
	UFUNCTION()
	void FadeOutAnimFinished();
};

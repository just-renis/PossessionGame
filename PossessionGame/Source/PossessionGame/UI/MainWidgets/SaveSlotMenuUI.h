#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveSlotMenuUI.generated.h"

class UModeSelectUI;
class UCloseButtonUI;
class UConfirmationPopupUI;

UCLASS()
class POSSESSIONGAME_API USaveSlotMenuUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void OpenUI();
	UFUNCTION()
	void CloseUI();
private:
	bool bIsConfirmationPanelOpen = false;
	bool bIsModeSelectionOpen = false;
	FString SaveSlotName;
	FString RunSaveSlotName;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UConfirmationPopupUI> ConfirmationPopup;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> SaveSlot1;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> SaveSlot2;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> SaveSlot3;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> SaveSlot4;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCloseButtonUI> CloseButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> FadeOutImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UModeSelectUI> ModeSelection;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeOutAnim;
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeInAnimFinishedDelegate;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	void Toggle(const bool bState);
	void OpenConfirmationPopup(const FString& InSlotName, const FString& InRunSlotName);
	void ToggleSaveSlots(const bool bState) const;
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
	UFUNCTION()
	void TriggerSelectionMenuFadeOut();
};

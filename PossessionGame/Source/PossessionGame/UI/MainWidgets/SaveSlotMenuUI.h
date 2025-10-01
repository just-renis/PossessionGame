#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PossessionGame/Utilities/Data Utilities/FLoadLevelData.h"
#include "SaveSlotMenuUI.generated.h"

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
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeOutAnim;
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeInAnimFinishedDelegate;
	UPROPERTY()
	FTimerHandle LoadLevelHandle;
	UPROPERTY()
	FLoadLevelData PendingLoadLevelData;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	void Toggle(const bool bState);
	void OpenConfirmationPopup(const FString& InSlotName, const FString& InRunSlotName);
	void ResetToDefaultPlayerInputMode() const;
	void ToggleSaveSlots(const bool bState) const;
	void LoadLevel() const;
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

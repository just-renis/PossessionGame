#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerLevelSystemUI.generated.h"

class UCloseButtonUI;
class UHorizontalBox;
class USaveSlot;
class UHorizontalBoxSlot;
enum class ELevelSystemUpgradeTexture : uint8;
enum class EPlayerLevelUpgradeId : uint8;
class UVerticalBox;

UCLASS()
class POSSESSIONGAME_API UPlayerLevelSystemUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void OpenUI();
	UFUNCTION()
	void CloseUI();
private:
	int32 PlayerLevelMaxAmount = 8;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> PlayerLevelOptionClass;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TMap<EPlayerLevelUpgradeId, FText> UpgradeDescriptions;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UObject> Font;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TArray<ELevelSystemUpgradeTexture> LevelOptionsTextureTypes;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TMap<ELevelSystemUpgradeTexture, TObjectPtr<UObject>> IconTextures;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> OptionsBox;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCloseButtonUI> CloseButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnim;
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeOutDelegate;
	
	TObjectPtr<UWidget> GenerateLevelText(const int32 InIndex) const;
	FString GetRichTag(const bool bIsNumberTag);

	virtual void NativeConstruct() override;
	void SetupPlayerOptions();
	void Toggle(const bool bState);
	void SetupLevelText(const TObjectPtr<UHorizontalBoxSlot>& InElement);
	void SetupFirstLevelOption(const TObjectPtr<UHorizontalBoxSlot>& InElement);
	void SetupSecondLevelOption(const TObjectPtr<UHorizontalBoxSlot>& InElement);
	void SetupLevelOptions(const int32 InIndex, const TObjectPtr<USaveSlot>& InSaveSlot, const TObjectPtr<UHorizontalBox>& InHorizontalBox);
	void SetupSpacerBetweenLevelOptions() const;
	void SetupLevelOptionsDescriptions();
	UFUNCTION()
	void Hide();
};

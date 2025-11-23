#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "UpgradeDetailsPanelUI.generated.h"

enum class EUpgradeUnit : uint8;
enum class EPoolObject : uint8;
class URunSaveSlot;
class UCodexData;
enum class EUpgradeType : uint8;
enum class EAbility : uint8;
class URichTextBlock;
enum class EUpgradeAttribute : uint8;
class UButton;

UCLASS()
class POSSESSIONGAME_API UUpgradeDetailsPanelUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetButtonVisibility(const bool bState) const;
	void SetButtonState(const bool bState) const;
	void Setup(const EUpgrade InUpgradeName, const FUpgradeChoicePair& InUpgradeChoicePair = FUpgradeChoicePair());
	UFUNCTION()
	void PlayDisappearAnim();
private:
	bool bHasBeenSelected = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 ChoiceIndex;
	UPROPERTY(EditDefaultsOnly, Category = "Textures", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UTexture2D>> NormalTextures;
	UPROPERTY(EditDefaultsOnly, Category = "Textures", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UTexture2D>> HoverTextures;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> PlayerUpgradeText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> EnemyUpgradeText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> UpgradeChoiceButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> UpgradeDetailsPanelOverlay;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PressAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> UnHoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> DisappearAnim;
	UPROPERTY()
	EUpgrade UpgradeName;
	UPROPERTY()
	EUpgradeAttribute SpecialAttribute = EUpgradeAttribute::None;
	UPROPERTY()
	FUpgradeChoicePair UpgradeChoice;

	ECharacterType GetCharacterByUpgradeName() const;
	EAbility GetAbilityByUpgradeName() const;
	FString SanitizeFloatValueToString(const float InValue);
	FString GetUpgradeUnitString(const EUpgradeUnit InUpgradeUnit);
	FText GetUpgradeTextFromSingularAttributeValue(const TPair<EUpgradeAttribute, float> InAttribute, const bool bForEnemy);
	FString GetRichTextTag(const bool bForEnemy) const;
	FText GetSpecialUpgradeText(const EUpgradeAttribute InUpgradeAttribute, const bool bForEnemy);
	FString GetUpgradeChangeString(const float InCurrentValueFromUpgrades, const float InBaseValue, const float InUpgradeValue, const EUpgradeType InUpgradeType, const EUpgradeUnit InUpgradeUnit);
	FString GetHealthUpgradeString(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute);
	FString GetAbilityUpgradeString(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute);
	FString GetDamageUpgradeString(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute);
	FString GetProjectileAmountUpgradeString(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute);
	FString GetProjectileSpeedUpgradeString(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute);
	FString GetCloneAmountUpgradeString(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute);
	FString GetCapUpgradeString(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute);
	FString GetSizeUpgradeString(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute);
	FString GetEffectDurationUpgradeString(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute);
	FString GetDamageReductionUpgradeString(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute);
	FString GetImpulseUpgradeString(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute);
	FString GetMiscDamageUpgradeString(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute);
	FString GetProjectileAmountByName(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute, const EPoolObject InProjectileName);
	FString GetProjectileSpeedByName(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute, const EPoolObject InProjectileName);
	FString GetProjectileDamageByName(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute, const EPoolObject InProjectileName);
	FString GetThrowableDamageByName(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute, const EPoolObject InThrowableName);
	FString GetCapUpgradeByName(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute, const EPoolObject InObjectName);
	FString GetSizeUpgradeByName(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute, const EPoolObject InObjectName);
	FString GetEffectUpgradeByName(const TObjectPtr<UCodexData>& InCodexData, const TObjectPtr<URunSaveSlot>& InRunSaveSlot, const bool bForPlayer, const TPair<EUpgradeAttribute, float> InAttribute, const EPoolObject InObjectName);

	virtual void NativeConstruct() override;
	void SetUpgradeText(const bool bIsPlayerUpgrade);
	void SetUpgradeChoiceBackground();
	UFUNCTION()
	void SelectUpgrade();
	UFUNCTION()
	void PlayHoverAnim();
	UFUNCTION()
	void PlayUnHoverAnim();
};

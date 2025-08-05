#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PossessionGame/Enums/EExplanation.h"
#include "ExplanationUI.generated.h"

enum class EExplanation : uint8;
class UTextBlock;
class UMainMenuButtonUI;

USTRUCT()
struct FExplanationData
{
	GENERATED_BODY()

	int32 GetPageAmount() const { return ExplanationTexts.Num(); }
	FText GetExplanationText(const int32 InIndex) const { return ExplanationTexts.IsValidIndex(InIndex) ? ExplanationTexts[InIndex] : FText(); }

	UPROPERTY(EditAnywhere)
	EExplanation ExplanationName;
	UPROPERTY(EditAnywhere, meta = (MultiLine))
	TArray<FText> ExplanationTexts;
};

UCLASS()
class POSSESSIONGAME_API UExplanationUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void OpenUI(const EExplanation InExplanationName);
	void CloseUI();
private:
	int32 CurrentPage = 0;
	UPROPERTY(EditDefaultsOnly, Category = "Explanations", meta = (AllowPrivateAccess = true))
	TMap<EExplanation, FExplanationData> Explanations;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> BackButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> FinishButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> NextPageButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ExplanationText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PageText;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FpsFadeInAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FpsFadeOutAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> AbilitiesUpgradeVariationFadeInAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> AbilitiesUpgradeVariationFadeOutAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> VariationSlideRightAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> VariationRightFadeOutAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> BackNextTextAnim;
	UPROPERTY()
	FExplanationData CurrentExplanationData;

	virtual void NativeConstruct() override;
	void Toggle(const bool bState);
	void ShowExplanation() const;
	void SetCurrentExplanation(const EExplanation InExplanationName);
	UFUNCTION()
	void NextPage();
	UFUNCTION()
	void BackPage();
	UFUNCTION()
	void Finish();
};

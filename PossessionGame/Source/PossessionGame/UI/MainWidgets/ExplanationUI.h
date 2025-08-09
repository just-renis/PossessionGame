#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PossessionGame/Enums/EExplanation.h"
#include "ExplanationUI.generated.h"

class UImage;
class UExplanationHighlight;
class UTextBlock;
class UMainMenuButtonUI;

USTRUCT()
struct FExplanationData
{
	GENERATED_BODY()

	int32 GetPageAmount() const { return ExplanationTexts.Num(); }
	EExplanation GetExplanationName() const { return ExplanationName; }
	FText GetExplanationText(const int32 InIndex) const { return ExplanationTexts.IsValidIndex(InIndex) ? ExplanationTexts[InIndex] : FText(); }
	TObjectPtr<UTexture2D> GetHighlightTexture(const int32 InCurrentPage)
	{
		if (const TObjectPtr<UTexture2D>* HighlightTexture = ExplanationHighlights.Find(InCurrentPage))
		{
			return *HighlightTexture;
		}
		return nullptr;
	}
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	EExplanation ExplanationName = EExplanation::FpsUI;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TMap<int32, TObjectPtr<UTexture2D>> ExplanationHighlights;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true, MultiLine))
	TArray<FText> ExplanationTexts;
};

UCLASS()
class POSSESSIONGAME_API UExplanationUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void OpenUI(const EExplanation InExplanationName);
private:
	int32 PreviousPage = -1;
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
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UExplanationHighlight> Highlight;
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
	UPROPERTY()
	FWidgetAnimationDynamicEvent FinishedFadeOutDelegate;

	virtual void NativeConstruct() override;
	void Toggle(const bool bState);
	void ShowExplanation();
	void SetCurrentExplanation(const EExplanation InExplanationName);
	void PlayExplanationAnim(const EExplanation InExplanationName);
	void CloseExplanationLogic();
	void ChangeButtonsStates(const bool bState) const;
	UFUNCTION()
	void NextPage();
	UFUNCTION()
	void BackPage();
	UFUNCTION()
	void CloseUI();
	UFUNCTION()
	void FinishedFadeOutAnim();
};

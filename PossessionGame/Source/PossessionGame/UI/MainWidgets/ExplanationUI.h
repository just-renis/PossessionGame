#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
	TArray<FText> ExplanationTexts;
};

UCLASS()
class POSSESSIONGAME_API UExplanationUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void OpenUI();
	void CloseUI();
private:
	int32 CurrentPage = 0;
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
	UPROPERTY(EditDefaultsOnly, Category = "Explanations", meta = (AllowPrivateAccess = true))
	TMap<EExplanation, FExplanationData> Explanations;
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

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChapterLaunchUI.generated.h"

class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UChapterLaunchUI : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ChapterText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TitleText;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnim;
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeInAnimFinishedDelegate;

	FText GetChapterName(const int32 InCurrentChapter);

	virtual void NativeConstruct() override;
	void SetTexts(const FText& InChapterText, const FText& InTitleText) const;
	UFUNCTION()
	void FadeInAnimFinished();
};

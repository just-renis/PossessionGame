#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChapterLaunchUI.generated.h"

UCLASS()
class POSSESSIONGAME_API UChapterLaunchUI : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnim;
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeInAnimFinishedDelegate;

	virtual void NativeConstruct() override;
	UFUNCTION()
	void FadeInAnimFinished();
};

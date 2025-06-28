#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingScreenUI.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class POSSESSIONGAME_API ULoadingScreenUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void Init(const TFunction<void()>& InCallback);
private:
	int32 TotalPreCompiles = 0;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> LoadingBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LoadingBarText;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeOutAnim;
	UPROPERTY()
	FTimerHandle PSOPrecachingHandle;
	UPROPERTY()
	FTimerHandle DestroyHandle;

	void SetDestroyTimer();
	void DestroyThis() const;
};

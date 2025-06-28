#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingScreenUI.generated.h"

struct FTFunctionCallback;
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
	FWidgetAnimationDynamicEvent FadeOutFinishedAnimDelegate;

	virtual void NativeConstruct() override;
	void PlayFadeInAnim();
	void PlayFadeOutAnim();
	void DestroyThis();
	UFUNCTION()
	void LoadingProcess(const FTFunctionCallback& InCallback);
};

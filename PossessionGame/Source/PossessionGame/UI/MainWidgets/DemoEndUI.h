#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DemoEndUI.generated.h"

class UTextBlock;
class UImage;
class UMainMenuButtonUI;

USTRUCT()
struct FRunTimeRankData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int32 TimeInSecondsMax;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> RankTexture;
};

UCLASS()
class POSSESSIONGAME_API UDemoEndUI : public UUserWidget
{
	GENERATED_BODY()

	int32 RunEndTime;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TArray<FRunTimeRankData> Ranks;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> ReplayButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> ReturnToMenuButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> RankImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TimeText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CoinsAmountText;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnim;
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeInFinishedDelegate;
	UPROPERTY()
	FTimerHandle PauseHandle;

	int32 GetRank();
	FString GetConvertedNumber(const int32 InNumber);

	virtual void NativeConstruct() override;
	void PauseGame() const;
	void SetupRankUI();
	void SetRankTexture();
	UFUNCTION()
	void FadeInFinished();
	UFUNCTION()
	void ReturnToMainMenu();
	UFUNCTION()
	void Replay();
};

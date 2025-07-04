#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameLaunchUI.generated.h"

UCLASS()
class POSSESSIONGAME_API UGameLaunchUI : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> GameLaunchAnimation;
	UPROPERTY()
	FWidgetAnimationDynamicEvent GameLaunchAnimEndDelegate;

	virtual void NativeConstruct() override;
	UFUNCTION()
	void GameLaunchAnimationFinished();
};

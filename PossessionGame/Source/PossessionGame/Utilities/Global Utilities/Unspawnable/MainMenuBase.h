#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuBase.generated.h"

class USettingsSave;
enum class EWidget : uint8;

UCLASS()
class POSSESSIONGAME_API AMainMenuBase : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Save Data", meta = (AllowPrivateAccess = true))
	TSubclassOf<USettingsSave> SettingsSaveClass;
	UPROPERTY(EditAnywhere, Category = "Widget Manager", meta = (AllowPrivateAccess = true))
	TMap<EWidget, TSubclassOf<UUserWidget>> WidgetClasses;

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void BeginPlay() override;
	void LoadSettingsData() const;
	void InitAfterPSOPrecache() const;
	void ShowLoadingScreen();
	void ShowGameLaunchUI();
};

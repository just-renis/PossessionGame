#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TutorialModeBase.generated.h"

enum class EWidget : uint8;
class USettingsSave;

UCLASS()
class POSSESSIONGAME_API ATutorialModeBase : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Save Data", meta = (AllowPrivateAccess = true))
	TSubclassOf<USettingsSave> SettingsSaveClass;
	UPROPERTY(EditAnywhere, Category = "Widget Manager", meta = (AllowPrivateAccess = true))
	TMap<EWidget, TSubclassOf<UUserWidget>> WidgetClasses;
	UPROPERTY()
	FTimerHandle PSOPrecachingHandle;

	virtual void BeginPlay() override;
	void LoadSettingsData() const;
	void InitAfterPSOPrecache() const;
};

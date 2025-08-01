#pragma once

#include "CoreMinimal.h"
#include "Engine/RectLight.h"
#include "TutorialLight.generated.h"

UCLASS()
class POSSESSIONGAME_API ATutorialLight : public ALight
{
	GENERATED_BODY()

	ATutorialLight();

	bool bIsOn = false;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<URectLightComponent> RectLight;
	UPROPERTY()
	FTimerHandle BlinkingHandle;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void SetupBlinking();
	void Blinking();
};

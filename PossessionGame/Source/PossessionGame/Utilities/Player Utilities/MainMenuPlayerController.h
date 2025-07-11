#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayerController.generated.h"

class UInputMappingContext;

UCLASS()
class POSSESSIONGAME_API AMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputMappingContext> PlayerMappingContext;

	virtual void SetupInputComponent() override;
	void HandleEscapeClick();
	void SetupUserSettings() const;
};

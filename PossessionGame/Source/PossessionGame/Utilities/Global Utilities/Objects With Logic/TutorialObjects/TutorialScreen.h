#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialScreen.generated.h"

UCLASS()
class POSSESSIONGAME_API ATutorialScreen : public AActor
{
	GENERATED_BODY()

	ATutorialScreen();
	
	UPROPERTY(EditDefaultsOnly, Category = "Mesh", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void ChangeToAlertedMode() const;
};

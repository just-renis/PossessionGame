#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialComputer.generated.h"

UCLASS()
class POSSESSIONGAME_API ATutorialComputer : public AActor // TUTORIAL DEVICES(COMPUTERS / TV's)
{
	GENERATED_BODY()

	ATutorialComputer();

	UPROPERTY(EditAnywhere, Category = "Sound", meta = (AllowPrivateAccess = true))
	bool AlertBeepingEnabled = false;
	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (AllowPrivateAccess = true))
	TObjectPtr<UAudioComponent> BeepingSound;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void ChangeToAlertedMode() const;
};

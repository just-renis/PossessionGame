#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialStartRoom.generated.h"

UCLASS()
class POSSESSIONGAME_API ATutorialStartRoom : public AActor
{
	GENERATED_BODY()

	ATutorialStartRoom();

	bool bIsLampsOn = false;
	UPROPERTY(EditDefaultsOnly, Category = "Mesh", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_Lamp;
	UPROPERTY()
	FTimerHandle BlinkingHandle;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void ChangeToAlertState();
	void Blink();
};

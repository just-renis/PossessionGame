#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialButton.generated.h"

class USphereComponent;

UCLASS()
class POSSESSIONGAME_API ATutorialButton : public AActor
{
	GENERATED_BODY()
	
	ATutorialButton();

	int32 PatternOpacityIterationIndex = 0;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> MeshFrame;
	UPROPERTY(EditDefaultsOnly, Category = "Collision", meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> Collision;
	UPROPERTY()
	FTimerHandle ProcessAffectedObjectHandle;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_AffectedObject;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void ButtonClicked();
	void SetupAffectedObjectLogic();
	void ProcessAffectedObjectLogic();
	UFUNCTION()
	void OverlapOccured(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

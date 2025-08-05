#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "TutorialButton.generated.h"

class USphereComponent;

UCLASS()
class POSSESSIONGAME_API ATutorialButton : public AActor
{
	GENERATED_BODY()
	
	ATutorialButton();

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 ButtonIndex;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> MeshFrame;
	UPROPERTY(EditDefaultsOnly, Category = "Collision", meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> Collision;
	UPROPERTY()
	TObjectPtr<AEnemy> OverlappedPlayer;
	UPROPERTY()
	FTimerHandle CheckIfStompedHandle;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void ButtonClicked();
	void StompCheck();
	UFUNCTION()
	void OverlapOccured(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OverlapEnded(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};

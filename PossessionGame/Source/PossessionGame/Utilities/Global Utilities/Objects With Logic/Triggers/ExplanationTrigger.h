#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplanationTrigger.generated.h"

class USphereComponent;
enum class EExplanation : uint8;

UCLASS()
class POSSESSIONGAME_API AExplanationTrigger : public AActor
{
	GENERATED_BODY()

	AExplanationTrigger();

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	EExplanation Explanation;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> Collision;
	
	virtual void BeginPlay() override;
	UFUNCTION()
	void Initiate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

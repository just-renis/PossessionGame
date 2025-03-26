#pragma once

#include "CoreMinimal.h"
#include "Gate.generated.h"

UCLASS()
class POSSESSIONGAME_API AGate : public AActor
{
	GENERATED_BODY()
public:
	void OpenGate();
private:
	AGate();

	float TimePassed = 0.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsTutorialGate = false;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> FirstGateSide;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> SecondGateSide;

	virtual void BeginPlay() override;
	UFUNCTION()
	void MoveGateSides();
};

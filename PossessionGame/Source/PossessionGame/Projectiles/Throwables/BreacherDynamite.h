#pragma once

#include "CoreMinimal.h"
#include "Throwable.h"
#include "BreacherDynamite.generated.h"

class UBoxComponent;

UCLASS()
class POSSESSIONGAME_API ABreacherDynamite : public AThrowable
{
	GENERATED_BODY()
public:
	virtual void Throw() override;
	virtual void Init(const bool bInIsPlayerObject) override;
private:
	ABreacherDynamite();

	float SpecialExplosionDelay = 0.3f;
	FTimerHandle StickHandle;
	FTimerHandle SpecialExplosionDelayHandle;
	FVector SavedSpecialExplosionLocation;
	UPROPERTY(EditDefaultsOnly, Category = "Dynamite", meta = (AllowPrivateAccess = true))
	FVector StickingBounds;
	UPROPERTY()
	TObjectPtr<UAudioComponent> ExplodeSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> StickSfx;

	void PrimaryExplode();
	void Explode();
	void PerformStickTrace();
};

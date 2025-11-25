#pragma once

#include "CoreMinimal.h"
#include "Throwable.h"
#include "BreacherBarrel.generated.h"

UCLASS()
class POSSESSIONGAME_API ABreacherBarrel : public AThrowable
{
	GENERATED_BODY()
public:
	void Explode();
	virtual void Disable(const bool bHide) override;
	virtual void Init(const bool bInIsPlayerObject) override;
private:
	ABreacherBarrel();

	bool bIsAlreadyExploding = false;
	UPROPERTY()
	TObjectPtr<UAudioComponent> ExplodeSfx;
	
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};

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
private:
	ABreacherBarrel();

	UPROPERTY()
	TObjectPtr<UAudioComponent> ExplodeSfx;
	
	virtual void BeginPlay() override;
};

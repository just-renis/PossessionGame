#pragma once

#include "CoreMinimal.h"
#include "Throwable.h"
#include "BreacherBarrel.generated.h"

enum class ESound : uint16;

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
	UPROPERTY()
	TMap<ESound, TObjectPtr<UAudioComponent>> Sounds;
	
	virtual void BeginPlay() override;
	void PlaySound(const ESound InSoundName);
};

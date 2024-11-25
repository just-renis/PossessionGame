#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavAreaModifier.generated.h"

class UBoxComponent;
class UNavModifierComponent;

UCLASS()
class POSSESSIONGAME_API ANavAreaModifier : public AActor
{
	GENERATED_BODY()
public:
	ANavAreaModifier();
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UNavModifierComponent> NavModifierComponent;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBoxComponent> Collision;
};

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PossessionGameInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API UPossessionGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	float GetTimeDilation() { return GlobalTimeDilation; }
private:
	UPROPERTY(EditAnywhere, Category = "Global Variables", meta = (AllowPrivateAccess = true))
	float GlobalTimeDilation = 1.f;

	virtual void Init() override;
};
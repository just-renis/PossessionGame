#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PossessionGameInstance.generated.h"

class USaveSlot;

UCLASS()
class POSSESSIONGAME_API UPossessionGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	float GetTimeDilation() const { return GlobalTimeDilation; }
	TObjectPtr<USaveSlot> GetSaveSlot() const { return PlayerSaveSlot; };
	
	void LoadPlayerSaveSlot();
private:
	UPROPERTY(EditDefaultsOnly, Category = "Global Variables", meta = (AllowPrivateAccess = true))
	float GlobalTimeDilation = 1.f;
	UPROPERTY()
	TObjectPtr<USaveSlot> PlayerSaveSlot;

	virtual void Init() override;
};
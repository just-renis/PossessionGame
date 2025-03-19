#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PA_VfxContainer.generated.h"

enum class EVfx : uint8;
class UNiagaraSystem;

UCLASS()
class POSSESSIONGAME_API UPA_VfxContainer : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	bool GetIsLoaded () const { return bIsLoaded; }
	TSoftObjectPtr<UNiagaraSystem>& GetVfxEffect() { return VfxEffect; }

	void LoadVfx(const TObjectPtr<UWorld>& InWorld, TFunction<void()> OnVfxLoaded);
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
private:
	bool bIsInLoad = false;
	bool bIsLoaded = false;
	UPROPERTY(EditAnywhere, Category = "Vfx Data", meta = (AllowPrivateAccess = true))
	FName VfxPrimaryName;
	UPROPERTY(EditAnywhere, Category = "Vfx", meta = (AllowPrivateAccess = true))
	TSoftObjectPtr<UNiagaraSystem> VfxEffect;
	UPROPERTY()
	TObjectPtr<UWorld> World;

	bool IsGameRunning() { return IsValid(this) && World && !World->HasAnyFlags(RF_BeginDestroyed | RF_FinishDestroyed); }
};

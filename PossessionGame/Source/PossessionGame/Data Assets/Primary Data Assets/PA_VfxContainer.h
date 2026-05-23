#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PossessionGame/Utilities/Data Utilities/FVariationVfxParams.h"
#include "PA_VfxContainer.generated.h"

enum class EVariation : uint8;
class UNiagaraSystem;

UCLASS()
class POSSESSIONGAME_API UPA_VfxContainer : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	FName GetVfxName() const { return VfxPrimaryName; }
	TSoftObjectPtr<UNiagaraSystem>& GetVfxEffect() { return VfxEffect; }
	TMap<FName, FNiagaraUserParam>* GetParams(const EVariation InVariationName);

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
private:
	UPROPERTY(EditAnywhere, Category = "Vfx Data", meta = (AllowPrivateAccess = true))
	FName VfxPrimaryName;
	UPROPERTY(EditAnywhere, Category = "Vfx", meta = (AllowPrivateAccess = true))
	TSoftObjectPtr<UNiagaraSystem> VfxEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Vfx Data", meta = (AllowPrivateAccess = true))
	TMap<EVariation, FVariationVfxParams> VariationParams;
};

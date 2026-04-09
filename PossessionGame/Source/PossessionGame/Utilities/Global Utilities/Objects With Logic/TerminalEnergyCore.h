#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerminalEnergyCore.generated.h"

class UPointLightComponent;
class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API ATerminalEnergyCore : public AActor
{
	GENERATED_BODY()
	
	ATerminalEnergyCore();

	int32 ColorSwitchParamIndex = 0;
	TArray<FLinearColor> VfxColors = { FLinearColor(0.5765f, 1.f, 0.f), FLinearColor(0.8902f, 1.f, 0.f), FLinearColor(1.f, 0.8039f, 0.0627f), FLinearColor(1.f, 0.051f, 0.f) };
	TArray<FLinearColor> BallColors = { FLinearColor(0.447f, 0.7804f, 0.0117f), FLinearColor(0.7765f, 0.7804f, 0.f), FLinearColor(0.7804f, 0.5804f, 0.f), FLinearColor(0.7804f, 0.1137f, 0.f) };
	TArray<FLinearColor> LightColors = { FLinearColor(0.8549f, 1.f, 0.7529f), FLinearColor(1.f, 0.9882f, 0.7529f), FLinearColor(1.f, 0.8588f, 0.6549f), FLinearColor(1.f, 0.6627f, 0.6627f) };
	UPROPERTY(EditDefaultsOnly, Category = "Meshes", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> BallMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Light", meta = (AllowPrivateAccess = true))
	TObjectPtr<UPointLightComponent> LightComponent;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> EnergyCoreVfx;

	virtual void BeginPlay() override;
	void Interact(const int32 InId);
};

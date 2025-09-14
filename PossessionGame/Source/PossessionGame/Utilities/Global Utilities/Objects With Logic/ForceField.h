#pragma once

#include "CoreMinimal.h"
#include "ForceField.generated.h"

enum class EGameMode : uint8;

UCLASS()
class POSSESSIONGAME_API AForceField : public AActor
{
	GENERATED_BODY()

	AForceField();

	int32 HintIndex = 0;
	float PatternMaxOpacity = 0.f;
	float PatternOpacityDivisor = 0.f;
	float DeploymentPercent = 0.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bAppearByGameModeInitiation = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bForTerminalHackRoom = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bWithDurationForceField = false;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bWithDurationForceField", AllowPrivateAccess = true))
	float StartLoopAfter = 0.f;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bWithDurationForceField", AllowPrivateAccess = true))
	float DurationUntilDisappear = 5.f;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bWithDurationForceField", AllowPrivateAccess = true))
	float DurationUntilAppear = 5.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 ForceFieldIndex = -1;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	EGameMode TerminalRoomMode;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> ForceField;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_Material;
	UPROPERTY()
	FTimerHandle DissolveHandle;
	UPROPERTY()
	FTimerHandle DurationHandle;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void DestroyThis();
	void StartDissolveForceField(const int32 InIndex);
	void AppearForceField(const EGameMode InGameMode);
	void ProcessDissolveForceField();
	void StartDeploymentOfForceField();
	void ProcessDeploymentOfForceField();
	void DeployOnTerminalHackInitiation(const EGameMode InTerminalRoomMode);
	void DissolveOnTerminalHackFinish(const EGameMode InTerminalRoomMode);
	void SetMaterialValuesToInvisible() const;
	void SetupDurationTimer();
	void DissolveForceField();
	void ProcessDissolveForceFieldWithoutDestroying();
};

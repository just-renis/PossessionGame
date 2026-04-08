#pragma once

#include "CoreMinimal.h"
#include "InteractableObject.h"
#include "Terminal.generated.h"

class ATutorialButton;
enum class EGameMode : uint8;
class APoolObject;

UCLASS()
class POSSESSIONGAME_API ATerminal : public AInteractableObject
{
	GENERATED_BODY()

	ATerminal();

	int32 RotationTimes = 0;
	FTimerHandle ValveRotationHandle;
	UPROPERTY(EditAnywhere, Category = "Mode", meta = (AllowPrivateAccess = true))
	EGameMode RoomMode;
	UPROPERTY(EditAnywhere, Category = "Individual Values | Button", meta = (AllowPrivateAccess = true))
	bool bIsButton = false;
	UPROPERTY(EditAnywhere, Category = "Individual Values | Button", meta = (EditCondition = "bIsButton", EditConditionHides, AllowPrivateAccess = true))
	TObjectPtr<ATutorialButton> Button;
	UPROPERTY(EditAnywhere, Category = "Individual Values | Valve", meta = (AllowPrivateAccess = true))
	bool bIsValve = false;
	UPROPERTY(EditAnywhere, Category = "Individual Values | Valve", meta = (EditCondition = "bIsValve", EditConditionHides, AllowPrivateAccess = true))
	FVector SmokePipeOffset;
	UPROPERTY(EditAnywhere, Category = "Individual Values | Valve", meta = (EditCondition = "bIsValve", EditConditionHides, AllowPrivateAccess = true))
	FVector RoomSmokeOffset;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;

	virtual TObjectPtr<UStaticMeshComponent> GetMesh() override { return Mesh; }
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Interact() override;
	virtual void SetupInteractionLogic() override;
	void ShowMarker() const;
	void EnableTerminal(const EGameMode InGameMode);
	void DisableTerminal();
	void ToggleInteractableWidget(const bool bState);
	void ProcessRotationOfValve();
	void InteractWithButtonId(const int32 InButtonId);
};

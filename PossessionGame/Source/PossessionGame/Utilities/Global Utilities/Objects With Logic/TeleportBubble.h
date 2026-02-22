#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TeleportBubble.generated.h"

enum class EGameMode : uint8;

UCLASS()
class POSSESSIONGAME_API ATeleportBubble : public AActor
{
	GENERATED_BODY()
public:
	bool IsCorrectGameMode(const EGameMode InGameMode) const { return GameMode == InGameMode; }
	
	void Teleport(const TObjectPtr<AActor>& InActor) const;
private:
	UPROPERTY(EditAnywhere, Category = "Default Settings", meta = (AllowPrivateAccess = true))
	EGameMode GameMode;

	virtual void BeginPlay() override;
};

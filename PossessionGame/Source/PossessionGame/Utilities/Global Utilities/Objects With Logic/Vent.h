#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Vent.generated.h"

UCLASS()
class POSSESSIONGAME_API AVent : public AActor
{
	GENERATED_BODY()

	AVent();

	UPROPERTY(EditDefaultsOnly, Category = "Skeleton", meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	TObjectPtr<UAnimationAsset> VentAnim;

	virtual void BeginPlay() override;
};

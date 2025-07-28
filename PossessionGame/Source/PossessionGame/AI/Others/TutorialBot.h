#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialBot.generated.h"

UENUM()
enum class ETutorialBotAnimState
{
	Idle,
	ThumbsUp,
	Wave,
	Tutorial1,
	Tutorial2,
	Tutorial3,
	Tutorial4,
	Tutorial5
};

UCLASS()
class POSSESSIONGAME_API ATutorialBot : public AActor
{
	GENERATED_BODY()

	ATutorialBot();

	TObjectPtr<UAnimationAsset> GetAnimation(const ETutorialBotAnimState InAnimState);

	UPROPERTY(EditDefaultsOnly, Category = "Skeleton", meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, Category = "Animations", meta = (AllowPrivateAccess = true))
	TMap<ETutorialBotAnimState, TObjectPtr<UAnimationAsset>> Animations;

	virtual void BeginPlay() override;
};

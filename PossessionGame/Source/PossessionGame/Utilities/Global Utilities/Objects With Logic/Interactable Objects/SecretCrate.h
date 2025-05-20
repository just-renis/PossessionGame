#pragma once

#include "CoreMinimal.h"
#include "InteractableObject.h"
#include "SecretCrate.generated.h"

enum class ESound : uint16;

UCLASS()
class POSSESSIONGAME_API ASecretCrate : public AInteractableObject
{
	GENERATED_BODY()

	ASecretCrate();

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> CrateMesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UAnimSequence> OpenAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UMaterialInterface>> RewardMaterials;
	UPROPERTY()
	TMap<ESound, TObjectPtr<UAudioComponent>> Sounds;
	
	virtual TObjectPtr<USkeletalMeshComponent> GetSkeletalMesh() override { return CrateMesh; }

	virtual void BeginPlay() override;
	virtual void Interact() override;
	void GiveReward();
	void PlaySound(const ESound InSoundName);
	void StopSound(const ESound InSoundName);
};

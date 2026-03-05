#pragma once

#include "CoreMinimal.h"
#include "InteractableObject.h"
#include "SecretCrate.generated.h"

enum class ESound : uint16;
enum class EVariation : uint8;

UCLASS()
class POSSESSIONGAME_API ASecretCrate : public AInteractableObject
{
	GENERATED_BODY()

	ASecretCrate();

	FTimerHandle OpenAnimFinishedHandle;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> CrateMesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UAnimSequence> OpenAnim;
	UPROPERTY()
	TMap<ESound, TObjectPtr<UAudioComponent>> Sounds;
	
	virtual TObjectPtr<USkeletalMeshComponent> GetSkeletalMesh() override { return CrateMesh; }

	virtual void BeginPlay() override;
	virtual void Interact() override;
	void GiveReward();
	void PlaySound(const ESound InSoundName);
	void StopSound(const ESound InSoundName);
	void OnSecretCrateAnimFinished() const;
};

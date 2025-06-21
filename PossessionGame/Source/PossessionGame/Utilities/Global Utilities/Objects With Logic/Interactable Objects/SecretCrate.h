#pragma once

#include "CoreMinimal.h"
#include "InteractableObject.h"
#include "SecretCrate.generated.h"

enum class ESound : uint16;
enum class EVariation : uint8;

UENUM()
enum class ERewardMaterial : uint8
{
	Health,
	Hijack,
	Xp
};

UCLASS()
class POSSESSIONGAME_API ASecretCrate : public AInteractableObject
{
	GENERATED_BODY()

	ASecretCrate();

	int32 ProcessRewardLoopValue = 0;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> CrateMesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UAnimSequence> OpenAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UMaterialInterface>> RewardMaterials;
	UPROPERTY(EditDefaultsOnly, Category = "Post Process Materials", meta = (AllowPrivateAccess = true))
	TMap<ERewardMaterial, TObjectPtr<UMaterialInterface>> PP_Materials;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> PP_Material;
	UPROPERTY()
	FTimerHandle ProcessRewardHandle;
	UPROPERTY()
	TMap<ESound, TObjectPtr<UAudioComponent>> Sounds;
	
	EVariation SetupReward(const int32 InIndex);
	virtual TObjectPtr<USkeletalMeshComponent> GetSkeletalMesh() override { return CrateMesh; }

	virtual void BeginPlay() override;
	virtual void Interact() override;
	void GiveReward();
	void PlaySound(const ESound InSoundName);
	void StopSound(const ESound InSoundName);
	void ProcessRewardAppear();
	void ProcessRewardDisappear();
};

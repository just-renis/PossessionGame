#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/EVfx.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Utilities/Data Utilities/FNiagaraAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/FAsyncHandle.h"
#include "PrimaryDataAssetsManager.generated.h"

struct FNiagaraVariationData;
class UNiagaraComponent;
enum class EVfxType : uint8;
enum class EPoolObject : uint8;
class UPA_VfxContainer;
class UPA_CharacterContainer;
class UPA_WeaponContainer;
class UPA_SoundContainer;

USTRUCT()
struct FPoolObjectPairWrapper
{
	GENERATED_BODY()
	
	TPair<EPoolObject, EVfxType> Pair;

	bool operator==(const FPoolObjectPairWrapper& Other) const
	{
		return Pair.Key == Other.Pair.Key && Pair.Value == Other.Pair.Value;
	}
};

USTRUCT()
struct FCharacterPairWrapper
{
	GENERATED_BODY()
	
	TPair<ECharacterType, EVfxType> Pair;

	bool operator==(const FCharacterPairWrapper& Other) const
	{
		return Pair.Key == Other.Pair.Key && Pair.Value == Other.Pair.Value;
	}
};

FORCEINLINE uint32 GetTypeHash(const FPoolObjectPairWrapper& PairWrapper)
{
	uint32 Hash1 = GetTypeHash(PairWrapper.Pair.Key);
	uint32 Hash2 = GetTypeHash(PairWrapper.Pair.Value);
	return HashCombine(Hash1, Hash2);
}

FORCEINLINE uint32 GetTypeHash(const FCharacterPairWrapper& PairWrapper)
{
	uint32 Hash1 = GetTypeHash(PairWrapper.Pair.Key);
	uint32 Hash2 = GetTypeHash(PairWrapper.Pair.Value);
	return HashCombine(Hash1, Hash2);
}

UCLASS()
class POSSESSIONGAME_API UPrimaryDataAssetsManager : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	bool IsAsyncLoadingFinished();
	bool GetCanAsyncLoadHappen() const { return bCanAsyncLoadHappen; }
	float GetVfxAttributeValue(const TObjectPtr<UNiagaraComponent>& InComponent, const FName InVfxAttributeName);
	EVfx GetProjectileVfxName(const EPoolObject InProjectileName, const EVfxType InVfxType);
	EVfx GetProjectileVfxName(const ECharacterType InCharacterType, const EVfxType InVfxType);
	EVfx GetCharacterVfxName(const ECharacterType InCharacterType, const EVfxType InVfxType);
	TObjectPtr<UPA_WeaponContainer>& GetWeaponData(EWeapon WeaponName);
	TObjectPtr<UPA_SoundContainer>& GetSoundData(ESound SoundName);
	TObjectPtr<UPA_CharacterContainer>& GetCharacterData(ECharacterType CharacterName);
	TObjectPtr<UPA_VfxContainer>& GetVfxData(EVfx VfxName);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	void Init();
	void AddAsyncHandle(TSharedPtr<FStreamableHandle>& Handle) { AsyncHandles.Add(FAsyncHandle(Handle)); }
	void DeleteLoadedHandles();
	void ConfigureSound(TObjectPtr<UAudioComponent> InAudioComp, ESound InSoundName);
	void PlaySound2D(const ESound InSoundName, const bool bStoreSoundRef = false);
	void RemoveStoredSound(const ESound InSoundName);
	//void GetVfx(const EVfx InVfxName, TObjectPtr<UNiagaraComponent>* InVfxComponent);
	void AttachOrLoadVfx(const EVfx InVfxName, const FNiagaraVariationData InNiagaraVariationData, TObjectPtr<UNiagaraComponent>* InVfxComponent, const TObjectPtr<USceneComponent>& InAttachTo, const FName InSocketName, const bool bDeactivate = false, const FVector& InLocation = FVector::ZeroVector, const FRotator& InRotation = FRotator::ZeroRotator, const EAttachLocation::Type InAttachType = EAttachLocation::SnapToTarget, const bool bAutoDestroy = false);
	void SpawnOrLoadVfx(const EVfx InVfxName, const FVector& InLocation = FVector::ZeroVector, const FRotator& InRotation = FRotator::ZeroRotator, const TMap<FName, FNiagaraAttribute>& InAttributes = TMap<FName, FNiagaraAttribute>());
	void ApplyAttributesToVfxComponent(const TMap<FName, FNiagaraAttribute>& InAttributes, const TObjectPtr<UNiagaraComponent>& Component);
	void ChangeVfxVariation(const TObjectPtr<UNiagaraComponent>& InVfxComponent, const FNiagaraVariationData InNiagaraVariationData);
private:
	bool bCanAsyncLoadHappen = true;
	UPROPERTY()
	TMap<EWeapon, TObjectPtr<UPA_WeaponContainer>> WeaponContainers;
	UPROPERTY()
	TMap<ESound, TObjectPtr<UPA_SoundContainer>> SoundContainers;
	UPROPERTY()
	TMap<ECharacterType, TObjectPtr<UPA_CharacterContainer>> CharacterContainers;
	UPROPERTY()
	TMap<EVfx, TObjectPtr<UPA_VfxContainer>> VfxContainers;
	UPROPERTY()
	TArray<FAsyncHandle> AsyncHandles;
	UPROPERTY()
	TMap<FPoolObjectPairWrapper, EVfx> ProjectileVfxCombinationsMap;
	UPROPERTY()
	TMap<FCharacterPairWrapper, EVfx> CharacterVfxCombinationsMap;
	UPROPERTY()
	TMap<ESound, TObjectPtr<UAudioComponent>> StoredSounds;

	bool IsGameRunning();
	EPoolObject GetProjectileNameByCharacterName(ECharacterType InCharacterType);
	TObjectPtr<UNiagaraComponent> AttachVfx(const EVfx InVfxName, const FNiagaraVariationData InNiagaraVariationData, const TObjectPtr<USceneComponent>& InAttachTo, const FName InSocketName, const FVector& InLocation, const FRotator& InRotation, const EAttachLocation::Type InAttachType, const bool bAutoDestroy);
	TObjectPtr<UNiagaraComponent> SpawnVfx(const EVfx InVfxName, const FVector& InLocation, const FRotator& InRotation);

	void SetupWeaponsPrimaryAssets();
	void SetupSoundsPrimaryAssets();
	void SetupCharactersPrimaryAssets();
	void SetupVfxsPrimaryAssets();
	void SetAndPlaySoundCallback(const TObjectPtr<UAudioComponent>& InAudioComp, ESound InSoundName);
	void SpawnVfx(const EVfx InVfxName, const FVector& InLocation, const FRotator& InRotation, const TMap<FName, FNiagaraAttribute>& InAttributes);
	template<typename T, typename U>
	void FillContainers(TMap<FName, T>& Map, TMap<T, TObjectPtr<U>>& Containers, FName Type);
};

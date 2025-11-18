#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/EVariation.h"
#include "PossessionGame/Enums/EVfx.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Utilities/Data Utilities/FAsyncHandle.h"
#include "PossessionGame/Utilities/Data Utilities/FNiagaraAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/FTFunctionCallback.h"
#include "PrimaryDataAssetsManager.generated.h"

class ATrickster;
enum class EWidget : uint8;
struct FSpawnableEnemyByTotalAmount;
enum class EGameMode : uint8;
class ASpawnLocation;
class AEnemy;
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

USTRUCT()
struct POSSESSIONGAME_API FGameModeSpawnLocations
{
	GENERATED_BODY()

	TArray<TObjectPtr<ASpawnLocation>> SpawnLocations;
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

USTRUCT()
struct POSSESSIONGAME_API FCharacterContainer
{
	GENERATED_BODY()

	FCharacterContainer() {}
	explicit FCharacterContainer(const TObjectPtr<UPA_CharacterContainer>& InCharacterContainer) { CharacterContainer = InCharacterContainer; }

	TObjectPtr<UPA_CharacterContainer> GetCharacterContainer() const { return CharacterContainer; }
private:
	UPROPERTY()
	TObjectPtr<UPA_CharacterContainer> CharacterContainer;
};

USTRUCT()
struct POSSESSIONGAME_API FSoundContainer
{
	GENERATED_BODY()

	FSoundContainer() {}
	explicit FSoundContainer(const TObjectPtr<UPA_SoundContainer>& InSoundContainer) { SoundContainer = InSoundContainer; }

	bool GetIsInLoad() const { return bIsInLoad; }
	bool GetIsLoaded() const { return bIsLoaded; }
	TObjectPtr<UPA_SoundContainer> GetSoundContainer() const { return SoundContainer; }
	
	void SetIsInLoad(const bool bState) { bIsInLoad = bState; }
	void SetIsLoaded(const bool bState) { bIsLoaded = bState; }
	void ResetLoadVariables()
	{
		SetIsLoaded(false);
		SetIsInLoad(false);
	}
	void TriggerSoundsCallbacks()
	{
		for (const FTFunctionCallback Callback : SoundsLoadedCallbacks) if (Callback.Function) Callback.Function();
		SoundsLoadedCallbacks.Empty();
	}
	void AddSoundLoadedCallback(const TFunction<void()>& InCallback) { SoundsLoadedCallbacks.Add(FTFunctionCallback(InCallback)); }
private:
	bool bIsInLoad = false;
	bool bIsLoaded = false;
	UPROPERTY()
	TObjectPtr<UPA_SoundContainer> SoundContainer;
	UPROPERTY()
	TArray<FTFunctionCallback> SoundsLoadedCallbacks;
};

USTRUCT()
struct POSSESSIONGAME_API FVfxContainer
{
	GENERATED_BODY()

	FVfxContainer() {}
	explicit FVfxContainer(const TObjectPtr<UPA_VfxContainer>& InVfxContainer) { VfxContainer = InVfxContainer; }

	bool GetIsInLoad() const { return bIsInLoad; }
	bool GetIsLoaded() const { return bIsLoaded; }
	TObjectPtr<UPA_VfxContainer> GetVfxContainer() const { return VfxContainer; }
	
	void SetIsInLoad(const bool bState) { bIsInLoad = bState; }
	void SetIsLoaded(const bool bState) { bIsLoaded = bState; }
	void ResetLoadVariables()
	{
		SetIsLoaded(false);
		SetIsInLoad(false);
	}
	void TriggerVfxsCallbacks()
	{
		for (const FTFunctionCallback Callback : VfxLoadedCallbacks) if (Callback.Function) Callback.Function();
		VfxLoadedCallbacks.Empty();
	}
	void AddVfxLoadedCallback(const TFunction<void()>& InCallback) { VfxLoadedCallbacks.Add(FTFunctionCallback(InCallback)); }
private:
	bool bIsInLoad = false;
	bool bIsLoaded = false;
	UPROPERTY()
	TObjectPtr<UPA_VfxContainer> VfxContainer;
	UPROPERTY()
	TArray<FTFunctionCallback> VfxLoadedCallbacks;
};

UCLASS()
class POSSESSIONGAME_API UPrimaryDataAssetsManager : public UGameInstanceSubsystem
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
	FSoundContainer& GetSoundData(const ESound InSoundName);
	FCharacterContainer& GetCharacterData(const ECharacterType InCharacterName);
	FVfxContainer& GetVfxData(const EVfx InVfxName);
	TObjectPtr<AEnemy> SpawnVariationReplacement(const ECharacterType InCharacter, const EWidget InWidgetName);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	void AddAsyncHandle(TSharedPtr<FStreamableHandle>& Handle) { AsyncHandles.Add(FAsyncHandle(Handle)); }
	void DeleteLoadedHandles();
	void ConfigureSound(TObjectPtr<UAudioComponent> InAudioComp, TFunction<bool()> InCallback, const ESound InSoundName, const bool bIsPlayer = true);
	void PlaySound2D(const ESound InSoundName, const bool bStoreSoundRef = false, const bool bIsPlayer = true);
	void RemoveStoredSound(const ESound InSoundName);
	void AttachVfx(const EVfx InVfxName, TFunction<bool()> InCallbackOfDeactivation, TObjectPtr<UNiagaraComponent>* InVfxComponent, const TObjectPtr<USceneComponent>& InAttachTo = nullptr, const FName InSocketName = NAME_None, const bool bDeactivate = false, const FVector& InLocation = FVector::ZeroVector, const FRotator& InRotation = FRotator::ZeroRotator, const EAttachLocation::Type InAttachType = EAttachLocation::SnapToTarget, const bool bAutoDestroy = false, const EVariation InVariationName = EVariation::Default, const TMap<FName, FNiagaraAttribute>& InAttributes = TMap<FName, FNiagaraAttribute>());
	void SpawnVfx(EVfx InVfxName, TFunction<bool()> InCallbackOfDeactivation, const FVector& InLocation = FVector::ZeroVector, const FRotator& InRotation = FRotator::ZeroRotator, const EVariation InVariationName = EVariation::Default, const TMap<FName, FNiagaraAttribute>& InAttributes = TMap<FName, FNiagaraAttribute>());
	void ChangeVfxVariation(const FVfxContainer& InVfxData, TFunction<bool()> InCallbackOfDeactivation, const TObjectPtr<UNiagaraComponent>& InVfxComponent, const bool bDeactivate, const EVariation InVariationName);
	void RequestSpawnEnemy(ECharacterType InCharacter, const EGameMode InGameMode, TFunction<void(const float OutEnemyMaxHp)> InCallBack, TFunction<bool()> InSpawnCallback);
	void RequestSpawnEnemy(const ECharacterType InCharacter, const FVector& InLocation, const float InHpPercentage);
	void Init(const bool bIsTutorialFinished, const TArray<FSpawnableEnemyByTotalAmount>& InSpawnableEnemiesByTotalAmount);
	void SetupSpawnLocations();
	void PlaySoundAtLocation(const ESound InSoundName, const FVector& InLocation, const bool bIsPlayer = true, const bool bStoreSoundRef = false);
	void RequestSpawnClone(const ECharacterType InCharacterName, const TObjectPtr<ATrickster>& InOwner, const int32 InAmount);
	void UpdateVfxAttributes(const TObjectPtr<UNiagaraComponent>& InVfxComponent, const TMap<FName, FNiagaraAttribute>& InAttributes);
private:
	bool bIsInitialized = false;
	bool bCanAsyncLoadHappen = true;
	bool bTutorialIsFinished = false;
	bool bIsCloneSpawnInProcess = false;
	int32 TotalSpawnedEnemies = 0;
	int32 NextSpawnableEnemyIndex = 0;
	UPROPERTY()
	TMap<EWeapon, TObjectPtr<UPA_WeaponContainer>> WeaponContainers;
	UPROPERTY()
	TMap<ESound, FSoundContainer> SoundContainers;
	UPROPERTY()
	TMap<ECharacterType, FCharacterContainer> CharacterContainers;
	UPROPERTY()
	TMap<EVfx, FVfxContainer> VfxContainers;
	UPROPERTY()
	TArray<FAsyncHandle> AsyncHandles;
	UPROPERTY()
	TSet<ECharacterType> EnemyTypes;
	UPROPERTY()
	TArray<FSpawnableEnemyByTotalAmount> SpawnableEnemiesByTotalAmount;
	UPROPERTY()
	TMap<FPoolObjectPairWrapper, EVfx> ProjectileVfxCombinationsMap;
	UPROPERTY()
	TMap<FCharacterPairWrapper, EVfx> CharacterVfxCombinationsMap;
	UPROPERTY()
	TMap<ESound, TObjectPtr<UAudioComponent>> StoredSounds;
	UPROPERTY()
	TMap<ECharacterType, TSubclassOf<AEnemy>> LoadedEnemies;
	UPROPERTY()
	TMap<EGameMode, FGameModeSpawnLocations> SpawnLocations;

	bool IsAsyncAssetWorldValid(const TWeakObjectPtr<UWorld>& InWorld);
	EPoolObject GetProjectileNameByCharacterName(ECharacterType InCharacterType);
	TObjectPtr<UNiagaraComponent> AttachAndReceiveVfx(const EVfx InVfxName, const TObjectPtr<USceneComponent>& InAttachTo, const FName InSocketName, const FVector& InLocation, const FRotator& InRotation, const EAttachLocation::Type InAttachType, const bool bAutoDestroy);
	TObjectPtr<UNiagaraComponent> SpawnAndReceiveVfx(const EVfx InVfxName, const FVector& InLocation, const FRotator& InRotation);
	EWeapon GetWeaponByCharacter(const ECharacterType InCharacter);
	EWeapon GetSingularWeapon(const EWeapon InWeapon);
	ECharacterType GetRandomCharacterToSpawn();

	void PrimaryInit();
	void SetupWeaponsPrimaryAssets();
	void SetupSoundsPrimaryAssets();
	void SetupCharactersPrimaryAssets();
	void SetupVfxsPrimaryAssets();
	void SetAndPlaySoundCallback(const FSoundContainer& SoundData, const TObjectPtr<UAudioComponent>& InAudioComp, TFunction<bool()> InCallback, const bool bIsPlayer);
	void SetupVfx(const FVfxContainer& InVfxData, TFunction<bool()> InCallbackOfDeactivation, const TObjectPtr<UNiagaraComponent>& InVfxComponent, const bool bDeactivate, const EVariation InVariationName, const TMap<FName, FNiagaraAttribute>& InAttributes);
	void AsyncLoadEnemy(const ECharacterType InCharacter, const EGameMode InGameMode, TFunction<void(const float OutEnemyMaxHp)> InCallBack, TFunction<bool()> InSpawnCallback);
	void AsyncLoadEnemyCallback(const EWeapon InSingularWeapon, const EWeapon InWeapon, const ECharacterType InCharacter, const EGameMode InGameMode, TFunction<void(const float OutEnemyMaxHp)> InCallBack);
	void SpawnEnemy(const ECharacterType InCharacter, const EGameMode InCurrentGameMode, TFunction<void(const float OutEnemyMaxHp)> InCallBack);
	void SpawnTutorialEnemy(const ECharacterType InCharacter, const EGameMode InGameMode);
	void AddSpawnableEnemiesToThePool();
	void ClearBeforeInit();
	void LoadVfx(FVfxContainer& InVfxData, const EVfx InVfxName, TFunction<bool()> InCallbackOfDeactivation, TObjectPtr<UNiagaraComponent>* InVfxComponent, const TObjectPtr<USceneComponent>& InAttachTo, const FName InSocketName, const bool bDeactivate, const FVector& InLocation, const FRotator& InRotation, const EAttachLocation::Type InAttachType, const bool bAutoDestroy, const EVariation InVariationName, const TMap<FName, FNiagaraAttribute>& InAttributes);
	void LoadVfx(FVfxContainer& InVfxData, const EVfx InVfxName, TFunction<bool()> InCallbackOfDeactivation, const FVector& InLocation, const FRotator& InRotation, const EVariation InVariationName, const TMap<FName, FNiagaraAttribute>& InAttributes);
	void RequestVfxLoad(FVfxContainer& InVfxData);
	void SetupSound2D(const FSoundContainer& InSoundData, const ESound InSoundName, const bool bIsPlayer, const bool bStoreSoundRef);
	void LoadSound2D(FSoundContainer& InSoundData, const ESound InSoundName, const bool bIsPlayer, const bool bStoreSoundRef);
	void LoadSound3D(FSoundContainer& InSoundData, TObjectPtr<UAudioComponent> InAudioComp, TFunction<bool()> InCallback, const bool bIsPlayer);
	void RequestSoundLoad(FSoundContainer& InSoundData);
	void LoadSoundAtLocation(FSoundContainer& InSoundData, const ESound InSoundName, const FVector& InLocation, const bool bIsPlayer, const bool bStoreSoundRef);
	void SetupSoundAtLocationAndPlay(const FSoundContainer& InSoundData, const ESound InSoundName, const FVector& InLocation, const bool bIsPlayer, const bool bStoreSoundRef);
	void AsyncLoadClone(const ECharacterType InCharacterName, const TObjectPtr<ATrickster>& InOwner, const int32 InAmount);
	void AsyncLoadCloneCallback(const EWeapon InSingularWeapon, const EWeapon InWeapon, const ECharacterType InCharacterName, const TObjectPtr<ATrickster>& InOwner, const int32 InAmount);
	void SpawnClone(const EWeapon InSingularWeapon, const ECharacterType InCharacterName, const TObjectPtr<ATrickster>& InOwner, const int32 InAmount);
	template<typename EnumType, typename ContainerType, typename AssetType>
	void FillContainers(TMap<FName, EnumType>& InMap, TMap<EnumType, ContainerType>& InContainers, const FName InType);
	template<typename EnumType, typename ContainerType>
	void FillContainers(TMap<FName, EnumType>& InMap, TMap<EnumType, TObjectPtr<ContainerType>>& InContainers, const FName InType);
};

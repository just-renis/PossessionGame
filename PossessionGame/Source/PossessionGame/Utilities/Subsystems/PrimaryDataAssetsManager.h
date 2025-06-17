#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/EVfx.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Utilities/Data Utilities/FNiagaraAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/FAsyncHandle.h"
#include "PossessionGame/Utilities/Data Utilities/FNiagaraVariationData.h"
#include "PrimaryDataAssetsManager.generated.h"

class AMarker;
struct FSpawnableEnemyByTotalAmount;
class UDelegateManager;
enum class EGameMode : uint8;
class ASpawnLocation;
class AEnemy;
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
	TObjectPtr<UPA_SoundContainer>& GetSoundData(ESound SoundName);
	TObjectPtr<UPA_CharacterContainer>& GetCharacterData(ECharacterType CharacterName);
	TObjectPtr<UPA_VfxContainer>& GetVfxData(EVfx VfxName);
	TSubclassOf<AMarker>& GetMarkerClass() { return MarkerClass; }
	TObjectPtr<AEnemy> SpawnVariationReplacement(const ECharacterType InCharacter);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	void AddAsyncHandle(TSharedPtr<FStreamableHandle>& Handle) { AsyncHandles.Add(FAsyncHandle(Handle)); }
	void DeleteLoadedHandles();
	void ConfigureSound(TObjectPtr<UAudioComponent> InAudioComp, TFunction<bool()> InCallback, ESound InSoundName);
	void PlaySound2D(const ESound InSoundName, const bool bStoreSoundRef = false);
	void RemoveStoredSound(const ESound InSoundName);
	void AttachVfx(const EVfx InVfxName, TFunction<bool()> InCallback, const FNiagaraVariationData& InNiagaraVariationData, TObjectPtr<UNiagaraComponent>* InVfxComponent, const TObjectPtr<USceneComponent>& InAttachTo = nullptr, const FName InSocketName = NAME_None, const bool bDeactivate = false, const TMap<FName, FNiagaraAttribute>& InAttributes = TMap<FName, FNiagaraAttribute>(), const FVector& InLocation = FVector::ZeroVector, const FRotator& InRotation = FRotator::ZeroRotator, const EAttachLocation::Type InAttachType = EAttachLocation::SnapToTarget, const bool bAutoDestroy = false);
	void SpawnVfx(const EVfx InVfxName, TFunction<bool()> InCallback, const FVector& InLocation = FVector::ZeroVector, const FRotator& InRotation = FRotator::ZeroRotator, const FNiagaraVariationData& InNiagaraVariationData = FNiagaraVariationData(), const TMap<FName, FNiagaraAttribute>& InAttributes = TMap<FName, FNiagaraAttribute>());
	void ApplyAttributesToVfxComponent(const TMap<FName, FNiagaraAttribute>& InAttributes, const TObjectPtr<UNiagaraComponent>& Component);
	void ChangeVfxVariation(const TObjectPtr<UNiagaraComponent>& InVfxComponent, const FNiagaraVariationData& InNiagaraVariationData);
	void RequestSpawnEnemy(ECharacterType InCharacter, const EGameMode InGameMode, const bool bCanEvolve, TFunction<void()> InCallBack);
	void Init(const bool bIsTutorialFinished, const TSubclassOf<AMarker>& InMarkerClass, const TObjectPtr<UDelegateManager>& InDelegateManager, const TArray<FSpawnableEnemyByTotalAmount>& InSpawnableEnemiesByTotalAmount);
	void SetupSpawnLocations();
private:
	bool bCanAsyncLoadHappen = true;
	bool bIsSpawnInProcess = false;
	bool bTutorialIsFinished = false;
	int32 TotalSpawnedEnemies = 0;
	int32 NextSpawnableEnemyIndex = 0;
	UPROPERTY()
	TSubclassOf<AMarker> MarkerClass; // To not put tons of references everywhere where i need it and also it doesn't work with pooling FUCK 3D UI WIDGETS.
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;
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

	bool IsGameRunning();
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
	void SetAndPlaySoundCallback(const TObjectPtr<UAudioComponent>& InAudioComp, TFunction<bool()> InCallback, ESound InSoundName);
	void SetupVfx(TFunction<bool()> InCallback, const FNiagaraVariationData& InNiagaraVariationData, const TObjectPtr<UNiagaraComponent>& InVfxComponent, const TMap<FName, FNiagaraAttribute>& InAttributes, const bool bDeactivate = false);
	void AsyncLoadEnemy(const ECharacterType InCharacter, const EGameMode InGameMode, const bool bCanEvolve, TFunction<void()> InCallBack);
	void AsyncLoadCallback(const EWeapon InSingularWeapon, const EWeapon InWeapon, const ECharacterType InCharacter, const EGameMode InGameMode, const bool bCanEvolve, TFunction<void()> InCallBack);
	void SpawnEnemy(const ECharacterType InCharacter, const EGameMode InCurrentGameMode, const bool bCanEvolve, TFunction<void()> InCallBack);
	void SpawnTutorialEnemy(const ECharacterType InCharacter, const EGameMode InGameMode);
	void AddSpawnableEnemiesToThePool();
	template<typename T, typename U>
	void FillContainers(TMap<FName, T>& Map, TMap<T, TObjectPtr<U>>& Containers, FName Type);
};

#include "PrimaryDataAssetsManager.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "PoolingManager.h"
#include "TutorialManager.h"
#include "WeaponManager.h"
#include "Components/AudioComponent.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Enemies/Characters/Trickster.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_CharacterContainer.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_SoundContainer.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_WeaponContainer.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_VfxContainer.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/Enums/EVfxType.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/Projectiles/Projectile.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/SpawnLocation.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

bool UPrimaryDataAssetsManager::IsAsyncLoadingFinished()
{
	DeleteLoadedHandles();
	return AsyncHandles.IsEmpty();
}

float UPrimaryDataAssetsManager::GetVfxAttributeValue(const TObjectPtr<UNiagaraComponent>& InComponent, const FName InVfxAttributeName)
{
	float RetrievedValue = 0.f;
	const FNiagaraUserRedirectionParameterStore& SystemParameters = InComponent->GetOverrideParameters();
	const FNiagaraVariable FloatVar(FNiagaraTypeDefinition::GetFloatDef(), InVfxAttributeName);
	SystemParameters.GetParameterValue(RetrievedValue, FloatVar);
	return RetrievedValue;
}

EVfx UPrimaryDataAssetsManager::GetProjectileVfxName(const EPoolObject InProjectileName, const EVfxType InVfxType)
{
	if (const EVfx* FoundVfx = ProjectileVfxCombinationsMap.Find(FPoolObjectPairWrapper({ InProjectileName, InVfxType }))) return *FoundVfx;
	return EVfx::None;
}

EVfx UPrimaryDataAssetsManager::GetProjectileVfxName(const ECharacterType InCharacterType, const EVfxType InVfxType)
{
	if (const EVfx* FoundVfx = ProjectileVfxCombinationsMap.Find(FPoolObjectPairWrapper({ GetProjectileNameByCharacterName(InCharacterType), InVfxType }))) return *FoundVfx;
	return EVfx::None;
}

EVfx UPrimaryDataAssetsManager::GetCharacterVfxName(const ECharacterType InCharacterType, const EVfxType InVfxType)
{
	if (const EVfx* FoundVfx = CharacterVfxCombinationsMap.Find(FCharacterPairWrapper({ InCharacterType, InVfxType }))) return *FoundVfx;
	return EVfx::None;
}

TObjectPtr<UPA_WeaponContainer>& UPrimaryDataAssetsManager::GetWeaponData(EWeapon WeaponName) { return WeaponContainers[WeaponName]; }

FSoundContainer& UPrimaryDataAssetsManager::GetSoundData(const ESound InSoundName) { return SoundContainers[InSoundName]; }

FCharacterContainer& UPrimaryDataAssetsManager::GetCharacterData(const ECharacterType InCharacterName) { return CharacterContainers[InCharacterName]; }

FVfxContainer& UPrimaryDataAssetsManager::GetVfxData(const EVfx InVfxName) { return VfxContainers[InVfxName]; }

TObjectPtr<AEnemy> UPrimaryDataAssetsManager::SpawnVariationReplacement(const ECharacterType InCharacter, const EWidget InWidgetName)
{
	if (const TSubclassOf<AEnemy>* Enemy = LoadedEnemies.Find(InCharacter))
	{
		return SpawnLocations[InWidgetName == EWidget::VariationsUI ? EGameMode::TutorialMode1 : EGameMode::TutorialMode2].SpawnLocations[0]->SpawnEnemy(*Enemy);
	}
	return nullptr;
}

void UPrimaryDataAssetsManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ProjectileVfxCombinationsMap =
	{
		{FPoolObjectPairWrapper({ EPoolObject::RangerProjectile, EVfxType::Trail }), EVfx::RangerBulletTrail },
		{FPoolObjectPairWrapper({ EPoolObject::RangerProjectile, EVfxType::Impact }), EVfx::RangerBulletImpact },
		{FPoolObjectPairWrapper({ EPoolObject::RangerProjectile, EVfxType::Muzzle }), EVfx::RangerMuzzle },
		{FPoolObjectPairWrapper({ EPoolObject::RangerProjectile, EVfxType::FpsTrail }), EVfx::RangerFpsBulletTrail },
		{FPoolObjectPairWrapper({ EPoolObject::RangerGrenadeLauncherProjectile, EVfxType::Impact }), EVfx::RangerGrenadeExplosion },
		{FPoolObjectPairWrapper({ EPoolObject::RangerGrenadeLauncherProjectile, EVfxType::Trail }), EVfx::RangerGrenadeTrail },
		{FPoolObjectPairWrapper({ EPoolObject::MinionProjectile, EVfxType::Muzzle }), EVfx::MinionMuzzle },
		{FPoolObjectPairWrapper({ EPoolObject::MinionProjectile, EVfxType::Trail }), EVfx::MinionBulletTrail },
		{FPoolObjectPairWrapper({ EPoolObject::MinionProjectile, EVfxType::Impact }), EVfx::MinionBulletImpact },
		{FPoolObjectPairWrapper({ EPoolObject::TricksterProjectile, EVfxType::Muzzle }), EVfx::TricksterMuzzle },
		{FPoolObjectPairWrapper({ EPoolObject::TricksterProjectile, EVfxType::FpsTrail }), EVfx::TricksterFpsBulletTrail },
		{FPoolObjectPairWrapper({ EPoolObject::TricksterProjectile, EVfxType::Trail }), EVfx::TricksterBulletTrail },
		{FPoolObjectPairWrapper({ EPoolObject::TricksterPoisonProjectile, EVfxType::Trail }), EVfx::TricksterPoisonGrenadeTrail },
		{FPoolObjectPairWrapper({ EPoolObject::TricksterEmpProjectile, EVfxType::Impact }), EVfx::TricksterEmpImpact },
		{FPoolObjectPairWrapper({ EPoolObject::TricksterEmpProjectile, EVfxType::Trail }), EVfx::TricksterEmpGrenadeTrail },
		{FPoolObjectPairWrapper({ EPoolObject::SpyProjectile, EVfxType::Trail }), EVfx::SpyBulletTrail },
		{FPoolObjectPairWrapper({ EPoolObject::SpyProjectile, EVfxType::Impact }), EVfx::SpyBulletImpact },
		{FPoolObjectPairWrapper({ EPoolObject::SpyProjectile, EVfxType::Muzzle }), EVfx::SpyMuzzle },
		{FPoolObjectPairWrapper({ EPoolObject::SpyProjectile, EVfxType::FpsTrail }), EVfx::SpyFpsBulletTrail },
		{FPoolObjectPairWrapper({ EPoolObject::TankProjectile, EVfxType::Trail }), EVfx::TankBulletTrail },
		{FPoolObjectPairWrapper({ EPoolObject::TankProjectile, EVfxType::Impact }), EVfx::TankBulletImpact },
		{FPoolObjectPairWrapper({ EPoolObject::TankProjectile, EVfxType::Muzzle }), EVfx::TankMuzzle },
		{FPoolObjectPairWrapper({ EPoolObject::TankProjectile, EVfxType::FpsTrail }), EVfx::TankFpsBulletTrail },
		{FPoolObjectPairWrapper({ EPoolObject::GuardianProjectile, EVfxType::Muzzle }), EVfx::GuardianMuzzle },
		{FPoolObjectPairWrapper({ EPoolObject::GuardianProjectile, EVfxType::Trail }), EVfx::GuardianBulletTrail },
		{FPoolObjectPairWrapper({ EPoolObject::GuardianProjectile, EVfxType::Impact }), EVfx::GuardianBulletImpact },
		{FPoolObjectPairWrapper({ EPoolObject::VectorBlastProjectile, EVfxType::Impact }), EVfx::VectorBlastImpact },
		{FPoolObjectPairWrapper({ EPoolObject::VectorBlastProjectile, EVfxType::Trail }), EVfx::VectorBlastTrail },
		{FPoolObjectPairWrapper({ EPoolObject::MinionBossProjectile, EVfxType::Trail }), EVfx::MinionBossBulletTrail },
		{FPoolObjectPairWrapper({ EPoolObject::WardenProjectile, EVfxType::Trail }), EVfx::WardenBlasterProjectile },
		{FPoolObjectPairWrapper({ EPoolObject::MinionBossProjectile, EVfxType::Muzzle }), EVfx::MinionBossMuzzle },
		{FPoolObjectPairWrapper({ EPoolObject::CloneProjectile, EVfxType::Impact }), EVfx::CloneBulletImpact },
		{FPoolObjectPairWrapper({ EPoolObject::BreacherProjectile, EVfxType::Trail }), EVfx::BreacherBulletTrail },
		{FPoolObjectPairWrapper({ EPoolObject::BreacherProjectile, EVfxType::Muzzle }), EVfx::BreacherMuzzle },
		{FPoolObjectPairWrapper({ EPoolObject::BreacherProjectile, EVfxType::Impact }), EVfx::BreacherBulletImpact }
	};
	CharacterVfxCombinationsMap =
	{
		{FCharacterPairWrapper({ ECharacterType::Guardian, EVfxType::Spawn }), EVfx::GuardianSpawn },
		{FCharacterPairWrapper({ ECharacterType::Ranger, EVfxType::Spawn }), EVfx::RangerSpawn },
		{FCharacterPairWrapper({ ECharacterType::Trickster, EVfxType::Spawn }), EVfx::TricksterSpawn },
		{FCharacterPairWrapper({ ECharacterType::Minion, EVfxType::Spawn }), EVfx::MinionSpawn },
		{FCharacterPairWrapper({ ECharacterType::Warden, EVfxType::Spawn }), EVfx::WardenSpawn },
		{FCharacterPairWrapper({ ECharacterType::MinionBoss, EVfxType::Spawn }), EVfx::MinionBossSpawn }
	//	{FCharacterPairWrapper({ ECharacterType::Breacher, EVfxType::Spawn }), EVfx::BreacherSpawn }
	};
	PrimaryInit();
}

void UPrimaryDataAssetsManager::DeleteLoadedHandles()
{
	for (int32 i = AsyncHandles.Num() - 1; i >= 0; i--)
	{
		if (AsyncHandles[i].Handle->HasLoadCompleted())
		{
			AsyncHandles[i].Handle->ReleaseHandle();
			AsyncHandles.RemoveAt(i);
		}
	}
	if (AsyncHandles.IsEmpty()) bCanAsyncLoadHappen = false;
}

void UPrimaryDataAssetsManager::ConfigureSound(TObjectPtr<UAudioComponent> InAudioComp, TFunction<bool()> InCallback, const ESound InSoundName, const bool bIsPlayer)
{
	if (!GetCanAsyncLoadHappen() || !InAudioComp) return;
	FSoundContainer& SoundData = GetSoundData(InSoundName);
	if (SoundData.GetSoundContainer()->Sound.IsValid()) SetAndPlaySoundCallback(SoundData, InAudioComp, InCallback, bIsPlayer);
	else
	{
		if (SoundData.GetIsInLoad()) return;
		SoundData.SetIsInLoad(true);
		LoadSound3D(SoundData, InAudioComp, InCallback, bIsPlayer);
	}
}

void UPrimaryDataAssetsManager::PlaySound2D(const ESound InSoundName, const bool bStoreSoundRef, const bool bIsPlayer)
{
	if (!GetCanAsyncLoadHappen()) return;
	FSoundContainer& SoundData = GetSoundData(InSoundName);
	if (SoundData.GetSoundContainer()->Sound.IsValid()) SetupSound2D(SoundData, InSoundName, bIsPlayer, bStoreSoundRef);
	else
	{
		if (SoundData.GetIsInLoad()) return;
		SoundData.SetIsInLoad(true);
		LoadSound2D(SoundData, InSoundName, bIsPlayer, bStoreSoundRef);
	}
}

void UPrimaryDataAssetsManager::RemoveStoredSound(const ESound InSoundName)
{
	if (const TObjectPtr<UAudioComponent>* AudioComponentRef = StoredSounds.Find(InSoundName))
	{
		if (IsValid(AudioComponentRef->Get())) AudioComponentRef->Get()->Stop();
		StoredSounds.Remove(InSoundName);
	}
}

void UPrimaryDataAssetsManager::AttachVfx(const EVfx InVfxName, TFunction<bool()> InCallbackOfDeactivation, TObjectPtr<UNiagaraComponent>* InVfxComponent, const TObjectPtr<USceneComponent>& InAttachTo, const FName InSocketName, const bool bDeactivate, const FVector& InLocation, const FRotator& InRotation, const EAttachLocation::Type InAttachType, const bool bAutoDestroy, const EVariation InVariationName, const TMap<FName, FNiagaraAttribute>& InAttributes)
{
	if (InVfxName == EVfx::None) return;
	FVfxContainer& VfxData = GetVfxData(InVfxName);
	if (VfxData.GetVfxContainer()->GetVfxEffect().IsValid())
	{
		if (InVfxComponent)
		{
			*InVfxComponent = AttachAndReceiveVfx(InVfxName, InAttachTo, InSocketName, InLocation, InRotation, InAttachType, bAutoDestroy);
			SetupVfx(VfxData, InCallbackOfDeactivation, *InVfxComponent, bDeactivate, InVariationName, InAttributes);
		}
		else SetupVfx(VfxData, InCallbackOfDeactivation, AttachAndReceiveVfx(InVfxName, InAttachTo, InSocketName, InLocation, InRotation, InAttachType, bAutoDestroy), bDeactivate, InVariationName, InAttributes);
	}
	else if (GetCanAsyncLoadHappen())
	{
		if (VfxData.GetIsInLoad()) return;
		VfxData.SetIsInLoad(true);
		LoadVfx(VfxData, InVfxName, InCallbackOfDeactivation, InVfxComponent, InAttachTo, InSocketName, bDeactivate, InLocation, InRotation, InAttachType, bAutoDestroy, InVariationName, InAttributes);
	}
}

void UPrimaryDataAssetsManager::SpawnVfx(EVfx InVfxName, TFunction<bool()> InCallbackOfDeactivation, const FVector& InLocation, const FRotator& InRotation, const EVariation InVariationName, const TMap<FName, FNiagaraAttribute>& InAttributes)
{
	if (InVfxName == EVfx::None) return;
	FVfxContainer& VfxData = GetVfxData(InVfxName);
	if (VfxData.GetVfxContainer()->GetVfxEffect().IsValid()) SetupVfx(VfxData, InCallbackOfDeactivation, SpawnAndReceiveVfx(InVfxName, InLocation, InRotation), false, InVariationName, InAttributes);
	else if (GetCanAsyncLoadHappen())
	{
		if (VfxData.GetIsInLoad()) return;
		VfxData.SetIsInLoad(true);
		LoadVfx(VfxData, InVfxName, InCallbackOfDeactivation, InLocation, InRotation, InVariationName, InAttributes);
	}
}

void UPrimaryDataAssetsManager::ChangeVfxVariation(const FVfxContainer& InVfxData, TFunction<bool()> InCallbackOfDeactivation, const TObjectPtr<UNiagaraComponent>& InVfxComponent, const bool bDeactivate, const EVariation InVariationName)
{
	if (InVfxComponent) InVfxComponent->DeactivateImmediate();
	SetupVfx(InVfxData, InCallbackOfDeactivation, InVfxComponent, bDeactivate, InVariationName, TMap<FName, FNiagaraAttribute>());
}

void UPrimaryDataAssetsManager::RequestSpawnEnemy(ECharacterType InCharacter, const EGameMode InGameMode, TFunction<void()> InCallBack, TFunction<bool()> InSpawnCallback)
{
	if (InCharacter == ECharacterType::Random) InCharacter = GetRandomCharacterToSpawn();
	!LoadedEnemies.Contains(InCharacter) ? AsyncLoadEnemy(InCharacter, InGameMode, InCallBack, InSpawnCallback) : SpawnEnemy(InCharacter, InGameMode, InCallBack);
	TotalSpawnedEnemies++;
	AddSpawnableEnemiesToThePool();
}

void UPrimaryDataAssetsManager::Init(const bool bIsTutorialFinished, const TArray<FSpawnableEnemyByTotalAmount>& InSpawnableEnemiesByTotalAmount)
{
	bTutorialIsFinished = bIsTutorialFinished;
	ClearBeforeInit();
	SetupSpawnLocations();
	if (!bTutorialIsFinished) GetWorld()->GetSubsystem<UTutorialManager>()->SpawnTutorialEnemy.BindUObject(this, &UPrimaryDataAssetsManager::SpawnTutorialEnemy);
	if (bIsInitialized) return;
	bIsInitialized = true;
	SpawnableEnemiesByTotalAmount = InSpawnableEnemiesByTotalAmount;
	if (SpawnableEnemiesByTotalAmount.Num() < 1) EnemyTypes.Add(ECharacterType::Ranger);
	else AddSpawnableEnemiesToThePool();
}

void UPrimaryDataAssetsManager::SetupSpawnLocations()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnLocation::StaticClass(), Actors);
	for (TObjectPtr<AActor> Actor : Actors)
	{
		TObjectPtr<ASpawnLocation> SpawnLocation = Cast<ASpawnLocation>(Actor);
		if (!SpawnLocations.Contains(SpawnLocation->GetGameMode())) SpawnLocations.Add(SpawnLocation->GetGameMode());
		SpawnLocations[SpawnLocation->GetGameMode()].SpawnLocations.Add(SpawnLocation);
	}
}

void UPrimaryDataAssetsManager::PlaySoundAtLocation(const ESound InSoundName, const FVector& InLocation, const bool bIsPlayer, const bool bStoreSoundRef)
{
	if (!GetCanAsyncLoadHappen()) return;
	FSoundContainer& SoundData = GetSoundData(InSoundName);
	if (SoundData.GetSoundContainer()->Sound.IsValid()) SetupSoundAtLocationAndPlay(SoundData, InSoundName, InLocation, bIsPlayer, bStoreSoundRef);
	else LoadSoundAtLocation(SoundData, InSoundName, InLocation, bIsPlayer, bStoreSoundRef);
}

void UPrimaryDataAssetsManager::RequestSpawnClone(const ECharacterType InCharacterName, const TObjectPtr<ATrickster>& InOwner, const int32 InAmount)
{
	if (bIsCloneSpawnInProcess) return;
	!LoadedEnemies.Contains(InCharacterName) ? AsyncLoadClone(InCharacterName, InOwner, InAmount) : SpawnClone(GetSingularWeapon(GetWeaponByCharacter(InCharacterName)), InCharacterName, InOwner, InAmount);
}

void UPrimaryDataAssetsManager::UpdateVfxAttributes(const TObjectPtr<UNiagaraComponent>& InVfxComponent, const TMap<FName, FNiagaraAttribute>& InAttributes)
{
	if (!InVfxComponent) return;
	for (const TPair<FName, FNiagaraAttribute> Attribute : InAttributes)
	{
		if (!Attribute.Value.bIsReplacingValue)
		{
			if (FMath::IsNearlyEqual(Attribute.Value.Value, 0.f, 0.001f)) continue;
			const float ComponentAttributeValue = GetVfxAttributeValue(InVfxComponent, Attribute.Key);
			const float AttributeValue = ComponentAttributeValue + ComponentAttributeValue * (Attribute.Value.Value * 0.01f);
			InVfxComponent->SetVariableFloat(Attribute.Key, AttributeValue);
		}
		else InVfxComponent->SetVariableFloat(Attribute.Key, Attribute.Value.Value);
	}
}

void UPrimaryDataAssetsManager::SetupSound2D(const FSoundContainer& InSoundData, const ESound InSoundName, const bool bIsPlayer, const bool bStoreSoundRef)
{
	float VolumeMultiplier = 1.f;
	if (InSoundData.GetSoundContainer()->bIsCharacterTypeDependant) VolumeMultiplier = bIsPlayer ? InSoundData.GetSoundContainer()->PlayerVolume : InSoundData.GetSoundContainer()->EnemyVolume;
	if (bStoreSoundRef)
	{
		StoredSounds.Add(InSoundName, UGameplayStatics::SpawnSound2D(GetGameInstance()->GetWorld(), InSoundData.GetSoundContainer()->Sound.Get(), VolumeMultiplier));
	}
	else UGameplayStatics::PlaySound2D(GetGameInstance()->GetWorld(), InSoundData.GetSoundContainer()->Sound.Get(), VolumeMultiplier);
}

void UPrimaryDataAssetsManager::LoadSound2D(FSoundContainer& InSoundData, const ESound InSoundName, const bool bIsPlayer, const bool bStoreSoundRef)
{
	InSoundData.AddSoundLoadedCallback([this, InSoundData, InSoundName, bIsPlayer, bStoreSoundRef] { SetupSound2D(InSoundData, InSoundName, bIsPlayer, bStoreSoundRef); });
	RequestSoundLoad(InSoundData);
}

void UPrimaryDataAssetsManager::LoadSound3D(FSoundContainer& InSoundData, TObjectPtr<UAudioComponent> InAudioComp, TFunction<bool()> InCallback, const bool bIsPlayer)
{
	InSoundData.AddSoundLoadedCallback([this, InSoundData, InAudioComp, InCallback, bIsPlayer] { SetAndPlaySoundCallback(InSoundData, InAudioComp, InCallback, bIsPlayer); });
	RequestSoundLoad(InSoundData);
}

void UPrimaryDataAssetsManager::RequestSoundLoad(FSoundContainer& InSoundData)
{
	if (!GetCanAsyncLoadHappen()) return;
	TWeakObjectPtr WeakWorld = GetWorld();
	FSoundContainer* SoundContainer = &InSoundData;
	TSharedPtr<FStreamableHandle> Handle = UAssetManager::GetStreamableManager().RequestAsyncLoad(InSoundData.GetSoundContainer()->Sound.ToSoftObjectPath(), [this, WeakWorld, SoundContainer]
	{
		if (!WeakWorld.IsValid() || !IsAsyncAssetWorldValid(WeakWorld.Get()) || !SoundContainer) return;
		SoundContainer->SetIsLoaded(true);
		SoundContainer->TriggerSoundsCallbacks();
	});
	AddAsyncHandle(Handle);
}

void UPrimaryDataAssetsManager::LoadSoundAtLocation(FSoundContainer& InSoundData, const ESound InSoundName, const FVector& InLocation, const bool bIsPlayer, const bool bStoreSoundRef)
{
	InSoundData.AddSoundLoadedCallback([this, InSoundData, InSoundName, InLocation, bIsPlayer, bStoreSoundRef] { SetupSoundAtLocationAndPlay(InSoundData, InSoundName, InLocation, bIsPlayer, bStoreSoundRef); });
	RequestSoundLoad(InSoundData);
}

void UPrimaryDataAssetsManager::SetupSoundAtLocationAndPlay(const FSoundContainer& InSoundData, const ESound InSoundName, const FVector& InLocation, const bool bIsPlayer, const bool bStoreSoundRef)
{
	float VolumeMultiplier = 1.f;
	if (InSoundData.GetSoundContainer()->bIsCharacterTypeDependant) VolumeMultiplier = bIsPlayer ? InSoundData.GetSoundContainer()->PlayerVolume : InSoundData.GetSoundContainer()->EnemyVolume;
	const TObjectPtr<UAudioComponent> AudioComponent = UGameplayStatics::SpawnSoundAtLocation(GetWorld(), InSoundData.GetSoundContainer()->Sound.Get(), InLocation, FRotator::ZeroRotator, VolumeMultiplier);
	if (bStoreSoundRef) StoredSounds.Add(InSoundName, AudioComponent);
	AudioComponent->Play();
}

void UPrimaryDataAssetsManager::AsyncLoadClone(const ECharacterType InCharacterName, const TObjectPtr<ATrickster>& InOwner, const int32 InAmount)
{
	bIsCloneSpawnInProcess = true;
	if (!GetCanAsyncLoadHappen()) return;
	const FCharacterContainer& CharacterContainer = GetCharacterData(InCharacterName);
	TArray<FSoftObjectPath> PathsToLoad;
	const TObjectPtr<UPA_CharacterContainer> CharContainer = CharacterContainer.GetCharacterContainer();
	if (!IsValid(CharContainer))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid CharacterContainer for character %s"), *UEnum::GetValueAsString(InCharacterName));
		return;
	}
	if (!CharContainer->CharacterClass.IsNull()) PathsToLoad.Add(CharContainer->CharacterClass.ToSoftObjectPath());
	else UE_LOG(LogTemp, Warning, TEXT("CharacterClass is invalid for character %s"), *UEnum::GetValueAsString(InCharacterName));
	if (!CharContainer->CharacterMesh.IsNull()) PathsToLoad.Add(CharContainer->CharacterMesh.ToSoftObjectPath());
	else UE_LOG(LogTemp, Warning, TEXT("CharacterMesh is invalid for character %s"), *UEnum::GetValueAsString(InCharacterName));
	EWeapon Weapon = GetWeaponByCharacter(InCharacterName);
	const TObjectPtr<UPA_WeaponContainer> WeaponContainer = GetWeaponData(Weapon);
	if (!IsValid(WeaponContainer))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid WeaponContainer for weapon %s"), *UEnum::GetValueAsString(Weapon));
		return;
	}
	if (!WeaponContainer->WeaponClass.IsNull()) PathsToLoad.Add(WeaponContainer->WeaponClass.ToSoftObjectPath());
	else UE_LOG(LogTemp, Warning, TEXT("WeaponClass is invalid for weapon %s"), *UEnum::GetValueAsString(Weapon));
	if (!WeaponContainer->HandsMesh.IsNull()) PathsToLoad.Add(WeaponContainer->HandsMesh.ToSoftObjectPath());
	else UE_LOG(LogTemp, Warning, TEXT("HandsMesh is invalid for weapon %s"), *UEnum::GetValueAsString(Weapon));
	EWeapon SingularWeapon = GetSingularWeapon(Weapon);
	if (SingularWeapon != EWeapon::None)
	{
		const TObjectPtr<UPA_WeaponContainer> SingularWeaponContainer = GetWeaponData(SingularWeapon);
		if (IsValid(SingularWeaponContainer))
		{
			if (!SingularWeaponContainer->WeaponClass.IsNull()) PathsToLoad.Add(SingularWeaponContainer->WeaponClass.ToSoftObjectPath());
			else UE_LOG(LogTemp, Warning, TEXT("SingularWeaponClass is invalid for weapon %s"), *UEnum::GetValueAsString(SingularWeapon));
			if (!SingularWeaponContainer->WeaponMesh.IsNull()) PathsToLoad.Add(SingularWeaponContainer->WeaponMesh.ToSoftObjectPath());
			else UE_LOG(LogTemp, Warning, TEXT("SingularWeaponMesh is invalid for weapon %s"), *UEnum::GetValueAsString(SingularWeapon));
		}
	}
	else if (!WeaponContainer->WeaponMesh.IsNull()) PathsToLoad.Add(WeaponContainer->WeaponMesh.ToSoftObjectPath());
	else UE_LOG(LogTemp, Warning, TEXT("WeaponMesh is invalid for weapon %s"), *UEnum::GetValueAsString(Weapon));
	TArray<FSoftObjectPath> ValidPaths;
	for (const FSoftObjectPath& Path : PathsToLoad) if (!Path.GetAssetPathString().IsEmpty()) ValidPaths.Add(Path);
	if (ValidPaths.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid paths to async load for character %s"), *UEnum::GetValueAsString(InCharacterName));
		return;
	}
	for (const FSoftObjectPath& Path : ValidPaths) UE_LOG(LogTemp, Warning, TEXT("Path: %s"), *Path.ToString());
	TWeakObjectPtr WeakWorld = GetWorld();
	TSharedPtr<FStreamableHandle> Handle = UAssetManager::GetStreamableManager().RequestAsyncLoad(ValidPaths, [this, WeakWorld, SingularWeapon, Weapon, InCharacterName, InOwner, InAmount]
	{
		if (!WeakWorld.IsValid() || !IsAsyncAssetWorldValid(WeakWorld.Get()))
		{
			UE_LOG(LogTemp, Warning, TEXT("World no longer valid during AsyncLoadClone callback for character %s"), *UEnum::GetValueAsString(InCharacterName));
			return;
		}
		AsyncLoadCloneCallback(SingularWeapon, Weapon, InCharacterName, InOwner, InAmount);
	});
	AddAsyncHandle(Handle);
}

void UPrimaryDataAssetsManager::AsyncLoadCloneCallback(const EWeapon InSingularWeapon, const EWeapon InWeapon, const ECharacterType InCharacterName, const TObjectPtr<ATrickster>& InOwner, const int32 InAmount)
{
	LoadedEnemies.Add(InCharacterName, Cast<UClass>(GetCharacterData(InCharacterName).GetCharacterContainer()->CharacterClass.Get()));
	if (InSingularWeapon != EWeapon::None)
	{
		GetGameInstance()->GetSubsystem<UWeaponManager>()->AddWeapon(InSingularWeapon, Cast<UClass>(GetWeaponData(InSingularWeapon)->WeaponClass.Get()));
	}
	GetGameInstance()->GetSubsystem<UWeaponManager>()->AddWeapon(InWeapon, Cast<UClass>(GetWeaponData(InWeapon)->WeaponClass.Get()));
	SpawnClone(InSingularWeapon, InCharacterName, InOwner, InAmount);
}

void UPrimaryDataAssetsManager::SpawnClone(const EWeapon InSingularWeapon, const ECharacterType InCharacterName, const TObjectPtr<ATrickster>& InOwner, const int32 InAmount)
{
	const TArray<FVector> CloneSpawnOffsets = InOwner->GetAvailableCloneSpawnOffsets(InAmount);
	for (int32 i = 0; i < CloneSpawnOffsets.Num(); i++)
	{
		const TObjectPtr<APoolObject> Clone = GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::Clone, CloneSpawnOffsets[i]);
		Clone->SetActorRotation(InOwner->GetActorRotation() - FRotator(0.f, 90.f, 0.f));
		Clone->InitClone(InOwner->GetIsHijacked(), GetCharacterData(InCharacterName).GetCharacterContainer()->CharacterMesh.LoadSynchronous(), GetWeaponData(InSingularWeapon)->WeaponMesh.LoadSynchronous());
	}
	bIsCloneSpawnInProcess = false;
}

bool UPrimaryDataAssetsManager::IsAsyncAssetWorldValid(const TWeakObjectPtr<UWorld>& InWorld)
{
	if (!InWorld.IsValid()) return false;
	const TObjectPtr<UWorld> World = InWorld.Get();
	return !World->HasAnyFlags(RF_BeginDestroyed | RF_FinishDestroyed);
}

EPoolObject UPrimaryDataAssetsManager::GetProjectileNameByCharacterName(ECharacterType InCharacterType)
{
	switch (InCharacterType)
	{
		case ECharacterType::Ranger: return EPoolObject::RangerProjectile;
		case ECharacterType::Minion: return EPoolObject::MinionProjectile;
		case ECharacterType::Trickster: return EPoolObject::TricksterProjectile;
		case ECharacterType::Breacher: return EPoolObject::BreacherProjectile;
		case ECharacterType::Wraith: return EPoolObject::WraithProjectile;
		case ECharacterType::Samurai: return EPoolObject::SamuraiProjectile;
		case ECharacterType::Spy: return EPoolObject::SpyProjectile;
		case ECharacterType::Tank: return EPoolObject::TankProjectile;
		case ECharacterType::Guardian: return EPoolObject::GuardianProjectile;
		case ECharacterType::MinionBoss: return EPoolObject::MinionBossProjectile;
		default: return EPoolObject::None;
	}
}

TObjectPtr<UNiagaraComponent> UPrimaryDataAssetsManager::AttachAndReceiveVfx(const EVfx InVfxName, const TObjectPtr<USceneComponent>& InAttachTo, const FName InSocketName, const FVector& InLocation, const FRotator& InRotation,  const EAttachLocation::Type InAttachType,  const bool bAutoDestroy)
{
	if (InAttachTo) return UNiagaraFunctionLibrary::SpawnSystemAttached(GetVfxData(InVfxName).GetVfxContainer()->GetVfxEffect().Get(), InAttachTo, InSocketName, InLocation, InRotation, InAttachType, bAutoDestroy);
	return UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, GetVfxData(InVfxName).GetVfxContainer()->GetVfxEffect().Get(), InLocation, InRotation);
}

TObjectPtr<UNiagaraComponent> UPrimaryDataAssetsManager::SpawnAndReceiveVfx(const EVfx InVfxName, const FVector& InLocation, const FRotator& InRotation)
{
	return UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, GetVfxData(InVfxName).GetVfxContainer()->GetVfxEffect().Get(), InLocation, InRotation);
}

EWeapon UPrimaryDataAssetsManager::GetWeaponByCharacter(const ECharacterType InCharacter)
{
	switch (InCharacter)
	{
		case ECharacterType::Tank: return EWeapon::TankWeapon;
		case ECharacterType::Minion: return EWeapon::MinionWeapon;
		case ECharacterType::Ranger: return EWeapon::RangerWeapon;
		case ECharacterType::Spy: return EWeapon::SpyWeapon;
		case ECharacterType::Trickster: return EWeapon::TricksterWeapon;
		case ECharacterType::Breacher: return EWeapon::BreacherWeapon;
		case ECharacterType::Wraith: return EWeapon::WraithWeapon;
		case ECharacterType::Samurai: return EWeapon::SamuraiWeapon;
		case ECharacterType::Guardian: return EWeapon::GuardianWeapon;
		case ECharacterType::MinionBoss: return EWeapon::MinionBossHandWeapon;
		case ECharacterType::Warden: return EWeapon::WardenBlasterWeapon;
		default: return EWeapon::None;
	}
}

EWeapon UPrimaryDataAssetsManager::GetSingularWeapon(const EWeapon InWeapon)
{
	switch (InWeapon)
	{
		case EWeapon::MinionWeapon: return EWeapon::MinionSingularWeapon;
		case EWeapon::TricksterWeapon: return EWeapon::TricksterSingularWeapon;
		case EWeapon::GuardianWeapon: return EWeapon::GuardianSingularWeapon;
		default: return EWeapon::None;
	}
}

ECharacterType UPrimaryDataAssetsManager::GetRandomCharacterToSpawn()
{
	const int32 TargetIndex = FMath::RandRange(0, EnemyTypes.Num() - 1);
	int32 CurrentIndex = 0;
	for (const ECharacterType EnemyType : EnemyTypes) if (CurrentIndex++ == TargetIndex) return EnemyType;
	return ECharacterType::Minion;
}

void UPrimaryDataAssetsManager::PrimaryInit()
{
	SetupWeaponsPrimaryAssets();
	SetupCharactersPrimaryAssets();
	SetupSoundsPrimaryAssets();
	SetupVfxsPrimaryAssets();
}

void UPrimaryDataAssetsManager::SetupWeaponsPrimaryAssets()
{
	TMap<FName, EWeapon> WeaponsMap;
	WeaponsMap.Add("PA_SpyWeapon", EWeapon::SpyWeapon);
	WeaponsMap.Add("PA_TricksterWeapon", EWeapon::TricksterWeapon);
	WeaponsMap.Add("PA_TricksterSingularWeapon", EWeapon::TricksterSingularWeapon);
	WeaponsMap.Add("PA_TankWeapon", EWeapon::TankWeapon);
	WeaponsMap.Add("PA_RangerWeapon", EWeapon::RangerWeapon);
	WeaponsMap.Add("PA_MinionWeapon", EWeapon::MinionWeapon);
	WeaponsMap.Add("PA_MinionSingularWeapon", EWeapon::MinionSingularWeapon);
	WeaponsMap.Add("PA_SamuraiWeapon", EWeapon::SamuraiWeapon);
	WeaponsMap.Add("PA_GuardianWeapon", EWeapon::GuardianWeapon);
	WeaponsMap.Add("PA_GuardianSingularWeapon", EWeapon::GuardianSingularWeapon);
	WeaponsMap.Add("PA_MinionBossHandWeapon", EWeapon::MinionBossHandWeapon);
	WeaponsMap.Add("PA_WardenBlasterWeapon", EWeapon::WardenBlasterWeapon);
	WeaponsMap.Add("PA_BreacherWeapon", EWeapon::BreacherWeapon);
	WeaponsMap.Add("PA_WraithWeapon", EWeapon::WraithWeapon);
	FillContainers(WeaponsMap, WeaponContainers, "Weapon");
	WeaponsMap.Empty();
}

void UPrimaryDataAssetsManager::SetupSoundsPrimaryAssets()
{
	TMap<FName, ESound> SoundMap;
	SoundMap.Add("PA_WaveStart", ESound::WaveStart);
	SoundMap.Add("PA_WaveCountdown", ESound::WaveCountdown);
	SoundMap.Add("PA_WaveCompleted", ESound::WaveCompleted);
	SoundMap.Add("PA_RangerShoot", ESound::RangerShoot);
	SoundMap.Add("PA_RangerReload", ESound::RangerReload);
	SoundMap.Add("PA_OnPossession", ESound::OnHijack);
	SoundMap.Add("PA_ReadyToPossess", ESound::ReadyToHijack);
	SoundMap.Add("PA_TakeDamageLayer1", ESound::TakeDamageLayer1);
	SoundMap.Add("PA_TakeDamageLayer2", ESound::TakeDamageLayer2);
	SoundMap.Add("PA_CriticalDamageBeepFps", ESound::CriticalDamageBeepFps);
	SoundMap.Add("PA_CriticalDamageBeepComputer", ESound::CriticalDamageBeepComputer);
	SoundMap.Add("PA_CriticalHealthEnter", ESound::CriticalHealthEnter);
	SoundMap.Add("PA_PlayerDeath", ESound::PlayerDeath);
	SoundMap.Add("PA_RangerOnPossession", ESound::RangerOnHijack);
	SoundMap.Add("PA_RangerSpawn", ESound::RangerSpawn);
	SoundMap.Add("PA_RangerDeath", ESound::RangerDeath);
	SoundMap.Add("PA_RangerDeployShield", ESound::RangerDeployShield);
	SoundMap.Add("PA_RangerDissolveShield", ESound::RangerDissolveShield);
	SoundMap.Add("PA_RangerGrenadeLaunch", ESound::RangerGrenadeLaunch);
	SoundMap.Add("PA_RangerGrenadeExplosion", ESound::RangerGrenadeExplosion);
	SoundMap.Add("PA_GuardianSlowMotion", ESound::GuardianSlowMotion);
	SoundMap.Add("PA_RangerGrenadeLaunchAim", ESound::RangerGrenadeLaunchAim);
	SoundMap.Add("PA_MinionShoot", ESound::MinionShoot);
	SoundMap.Add("PA_MinionSpawn", ESound::MinionSpawn);
	SoundMap.Add("PA_MinionDeath", ESound::MinionDeath);
	SoundMap.Add("PA_TricksterShoot", ESound::TricksterShoot);
	SoundMap.Add("PA_TricksterReload", ESound::TricksterReload);
	SoundMap.Add("PA_TricksterSpawn", ESound::TricksterSpawn);
	SoundMap.Add("PA_TricksterDeath", ESound::TricksterDeath);
	SoundMap.Add("PA_TricksterOnPossession", ESound::TricksterOnHijack);
	SoundMap.Add("PA_TricksterGrenadeThrow", ESound::TricksterGrenadeThrow);
	SoundMap.Add("PA_TricksterPoisonImpact", ESound::TricksterPoisonImpact);
	SoundMap.Add("PA_TricksterPoisonRelease", ESound::TricksterPoisonRelease);
	SoundMap.Add("PA_TricksterEmpImpact", ESound::TricksterEmpImpact);
	SoundMap.Add("PA_DamageTick", ESound::DamageTick);
	SoundMap.Add("PA_CriticalDamageTick", ESound::CriticalDamageTick);
	SoundMap.Add("PA_GuardianShoot", ESound::GuardianShoot);
	SoundMap.Add("PA_GuardianSpawn", ESound::GuardianSpawn);
	SoundMap.Add("PA_GuardianDeath", ESound::GuardianDeath);
	SoundMap.Add("PA_GuardianOverChargeStart", ESound::GuardianOverChargeStart);
	SoundMap.Add("PA_GuardianOverChargeEnd", ESound::GuardianOverChargeEnd);
	SoundMap.Add("PA_MinionBossStomp", ESound::MinionBossStomp);
	SoundMap.Add("PA_MinionBossShoot", ESound::MinionBossShoot);
	SoundMap.Add("PA_MinionBossLaserEyeCharge", ESound::MinionBossLaserEyeCharge);
	SoundMap.Add("PA_MinionBossLaserEyeHold", ESound::MinionBossLaserEyeHold);
	SoundMap.Add("PA_MinionBossDeathExplosion", ESound::MinionBossDeathExplosion);
	SoundMap.Add("PA_MinionBossElectrificationDeathPassive", ESound::MinionBossElectrificationDeathPassive);
	SoundMap.Add("PA_MinionBossDeath", ESound::MinionBossDeath);
	SoundMap.Add("PA_MinionBossSpawn", ESound::MinionBossSpawn);
	SoundMap.Add("PA_MinionBossTakeDamage", ESound::MinionBossTakeDamage);
	SoundMap.Add("PA_DialogueStart", ESound::DialogueStart);
	SoundMap.Add("PA_RestSoundTrack", ESound::RestSoundTrack);
	SoundMap.Add("PA_BattleModeSoundTrack", ESound::BattleModeSoundTrack);
	SoundMap.Add("PA_TutorialIntroSoundTrack", ESound::TutorialIntroSoundTrack);
	SoundMap.Add("PA_TutorialChaosSoundTrack", ESound::TutorialChaosSoundTrack);
	SoundMap.Add("PA_TutorialIntroBattleSoundTrack", ESound::TutorialIntroBattleSoundTrack);
	SoundMap.Add("PA_ArenaModeBattleSoundTrack", ESound::ArenaModeBattleSoundTrack);
	SoundMap.Add("PA_ArenaModeRestSoundTrack", ESound::ArenaModeRestSoundTrack);
	SoundMap.Add("PA_ButtonHover", ESound::ButtonHover);
	SoundMap.Add("PA_ButtonSelect", ESound::ButtonSelect);
	SoundMap.Add("PA_UpgradeCardSelect", ESound::UpgradeCardSelect);
	SoundMap.Add("PA_UpgradePanelFadeIn", ESound::UpgradePanelFadeIn);
	SoundMap.Add("PA_UpgradePanelFadeOut", ESound::UpgradePanelFadeOut);
	SoundMap.Add("PA_UpgradeChoicesFadeIn", ESound::UpgradeChoicesFadeIn);
	SoundMap.Add("PA_TaskAnnouncement", ESound::TaskAnnouncement);
	SoundMap.Add("PA_LevelUp", ESound::LevelUp);
	SoundMap.Add("PA_GeneratorCompleted", ESound::GeneratorCompleted);
	SoundMap.Add("PA_GeneratorInitiate", ESound::GeneratorInitiate);
	SoundMap.Add("PA_GeneratorDisableAnnouncement", ESound::GeneratorDisableAnnouncement);
	SoundMap.Add("PA_UsbCollect", ESound::UsbCollect);
	SoundMap.Add("PA_UsbInsert", ESound::UsbInsert);
	SoundMap.Add("PA_GatesOpen", ESound::GatesOpen);
	SoundMap.Add("PA_ForceFieldDissolve", ESound::ForceFieldDissolve);
	SoundMap.Add("PA_ForceFieldDissolveWarning", ESound::ForceFieldDissolveWarning);
	SoundMap.Add("PA_QuickMelee", ESound::QuickMelee);
	SoundMap.Add("PA_SecretCrateOpen", ESound::SecretCrateOpen);
	SoundMap.Add("PA_SecretCratePassive", ESound::SecretCratePassive);
	SoundMap.Add("PA_SecretCrateReceivePowerUp", ESound::SecretCrateReceivePowerUp);
	SoundMap.Add("PA_PlayerStompLand", ESound::PlayerStompLand);
	SoundMap.Add("PA_PlayerStompDashDown", ESound::PlayerStompDashDown);
	SoundMap.Add("PA_Jump", ESound::Jump);
	SoundMap.Add("PA_DoubleJump", ESound::DoubleJump);
	SoundMap.Add("PA_MechanicalDeath", ESound::MechanicalDeath);
	SoundMap.Add("PA_EmpEffectApplied", ESound::EmpEffectApplied);
	SoundMap.Add("PA_ClickArrow", ESound::ClickArrow);
	SoundMap.Add("PA_PanelFadeIn", ESound::PanelFadeIn);
	SoundMap.Add("PA_PanelFadeOut", ESound::PanelFadeOut);
	SoundMap.Add("PA_NotSelectable", ESound::NotSelectable);
	SoundMap.Add("PA_OutOfAmmo", ESound::OutOfAmmo);
	SoundMap.Add("PA_FinishOffEnemies", ESound::FinishOffEnemies);
	SoundMap.Add("PA_AbilityReset", ESound::AbilityReset);
	SoundMap.Add("PA_AbilitySelect", ESound::AbilitySelect);
	SoundMap.Add("PA_BotHitVent", ESound::BotHitVent);
	SoundMap.Add("PA_BotBreakVent", ESound::BotBreakVent);
	SoundMap.Add("PA_BotLandOnHead", ESound::BotLandOnHead);
	SoundMap.Add("PA_BotFootStep", ESound::BotFootStep);
	SoundMap.Add("PA_BotPressTablet", ESound::BotPressTablet);
	SoundMap.Add("PA_ErrorChaos", ESound::ErrorChaos);
	SoundMap.Add("PA_AlertSignalization", ESound::AlertSignalization);
	SoundMap.Add("PA_DoorBreak", ESound::DoorBreak);
	SoundMap.Add("PA_Slide", ESound::Slide);
	SoundMap.Add("PA_EndScreenFadeIn", ESound::EndScreenFadeIn);
	SoundMap.Add("PA_CloneSpawn", ESound::CloneSpawn);
	SoundMap.Add("PA_CloneDisappear", ESound::CloneDisappear);
	SoundMap.Add("PA_TricksterCloneDash", ESound::TricksterCloneDash);
	SoundMap.Add("PA_RangerDeployMine", ESound::RangerDeployMine);
	SoundMap.Add("PA_RangerThrowMine", ESound::RangerThrowMine);
	SoundMap.Add("PA_RangerDetonateMine", ESound::RangerDetonateMine);
	SoundMap.Add("PA_VectorBlastBounce", ESound::VectorBlastBounce);
	SoundMap.Add("PA_VectorBlastHit", ESound::VectorBlastHit);
	SoundMap.Add("PA_VectorBlastLaunch", ESound::VectorBlastLaunch);
	SoundMap.Add("PA_VectorBlastSpecialImpactUpgrade", ESound::VectorBlastSpecialImpactUpgrade);
	SoundMap.Add("PA_WardenDeath", ESound::WardenDeath);
	SoundMap.Add("PA_WardenSpawn", ESound::WardenSpawn);
	SoundMap.Add("PA_WardenBeamFire", ESound::WardenBeamFire);
	SoundMap.Add("PA_WardenShoot", ESound::WardenShoot);
	SoundMap.Add("PA_JumpPadActivate", ESound::JumpPadActivate);
	SoundMap.Add("PA_BreacherOnHijack", ESound::BreacherOnHijack);
	SoundMap.Add("PA_BreacherDeath", ESound::BreacherDeath);
	SoundMap.Add("PA_BreacherSpawn", ESound::BreacherSpawn);
	SoundMap.Add("PA_BreacherShoot", ESound::BreacherShoot);
	SoundMap.Add("PA_BreacherReload", ESound::BreacherReload);
	SoundMap.Add("PA_BreacherThrowBarrel", ESound::BreacherThrowBarrel);
	SoundMap.Add("PA_BreacherBarrelExplosion", ESound::BreacherBarrelExplosion);
	FillContainers<ESound, FSoundContainer, UPA_SoundContainer>(SoundMap, SoundContainers, "Sound");
	SoundMap.Empty();
}

void UPrimaryDataAssetsManager::SetupCharactersPrimaryAssets()
{
	TMap<FName, ECharacterType> CharactersMap;
	CharactersMap.Add("PA_Spy", ECharacterType::Spy);
	CharactersMap.Add("PA_Trickster", ECharacterType::Trickster);
	CharactersMap.Add("PA_Breacher", ECharacterType::Breacher);
	CharactersMap.Add("PA_Wraith", ECharacterType::Wraith);
	CharactersMap.Add("PA_Tank", ECharacterType::Tank);
	CharactersMap.Add("PA_Ranger", ECharacterType::Ranger);
	CharactersMap.Add("PA_Minion", ECharacterType::Minion);
	CharactersMap.Add("PA_Samurai", ECharacterType::Samurai);
	CharactersMap.Add("PA_Guardian", ECharacterType::Guardian);
	CharactersMap.Add("PA_MinionBoss", ECharacterType::MinionBoss);
	CharactersMap.Add("PA_Warden", ECharacterType::Warden);
	FillContainers<ECharacterType, FCharacterContainer, UPA_CharacterContainer>(CharactersMap, CharacterContainers, "Character");
	CharactersMap.Empty();
}

void UPrimaryDataAssetsManager::SetupVfxsPrimaryAssets()
{
	TMap<FName, EVfx> VfxsMap;
	VfxsMap.Add("PA_RangerMuzzle", EVfx::RangerMuzzle);
	VfxsMap.Add("PA_RangerBulletTrail", EVfx::RangerBulletTrail);
	VfxsMap.Add("PA_RangerBulletImpact", EVfx::RangerBulletImpact);
	VfxsMap.Add("PA_RangerFpsBulletTrail", EVfx::RangerFpsBulletTrail);
	VfxsMap.Add("PA_RangerGrenadeExplosion", EVfx::RangerGrenadeExplosion);
	VfxsMap.Add("PA_RangerGrenadeLauncherMuzzle", EVfx::RangerGrenadeLauncherMuzzle);
	VfxsMap.Add("PA_RangerGrenadeLauncherMarker", EVfx::RangerGrenadeLauncherMarker);
	VfxsMap.Add("PA_RangerDeathElectrification", EVfx::RangerDeathElectrification);
	VfxsMap.Add("PA_RangerGrenadeTrail", EVfx::RangerGrenadeTrail);
	VfxsMap.Add("PA_RangerGrenadeLaunchSpecial", EVfx::RangerGrenadeLaunchSpecial);
	VfxsMap.Add("PA_RangerSpawn", EVfx::RangerSpawn);
	VfxsMap.Add("PA_RangerMineRadius", EVfx::RangerMineRadius);
	VfxsMap.Add("PA_RangerMineImpact", EVfx::RangerMineImpact);
	VfxsMap.Add("PA_RangerMineExplosion", EVfx::RangerMineExplosion);
	VfxsMap.Add("PA_RangerMineSpecialRadius", EVfx::RangerMineSpecialRadius);
	VfxsMap.Add("PA_MinionMuzzle", EVfx::MinionMuzzle);
	VfxsMap.Add("PA_MinionBulletTrail", EVfx::MinionBulletTrail);
	VfxsMap.Add("PA_MinionBulletImpact", EVfx::MinionBulletImpact);
	VfxsMap.Add("PA_MinionSpawn", EVfx::MinionSpawn);
	VfxsMap.Add("PA_MinionDeathElectrification", EVfx::MinionDeathElectrification);
	VfxsMap.Add("PA_TricksterMuzzle", EVfx::TricksterMuzzle);
	VfxsMap.Add("PA_TricksterFpsBulletTrail", EVfx::TricksterFpsBulletTrail);
	VfxsMap.Add("PA_TricksterBulletTrail", EVfx::TricksterBulletTrail);
	VfxsMap.Add("PA_TricksterPoisonImpact", EVfx::TricksterPoisonImpact);
	VfxsMap.Add("PA_TricksterPoison", EVfx::TricksterPoison);
	VfxsMap.Add("PA_AITricksterPoison", EVfx::AI_TricksterPoison);
	VfxsMap.Add("PA_AITricksterPoisonImpact", EVfx::AI_TricksterPoisonImpact);
	VfxsMap.Add("PA_AITricksterCloneAbilityRun", EVfx::AI_TricksterCloneAbilityRun);
	VfxsMap.Add("PA_TricksterEmpImpact", EVfx::TricksterEmpImpact);
	VfxsMap.Add("PA_TricksterReload", EVfx::TricksterReload);
	VfxsMap.Add("PA_TricksterDeathElectrification", EVfx::TricksterDeathElectrification);
	VfxsMap.Add("PA_TricksterEmpGrenadeTrail", EVfx::TricksterEmpGrenadeTrail);
	VfxsMap.Add("PA_TricksterPoisonGrenadeTrail", EVfx::TricksterPoisonGrenadeTrail);
	VfxsMap.Add("PA_TricksterSpawn", EVfx::TricksterSpawn);
	VfxsMap.Add("PA_TricksterGrenadeLauncherMarker", EVfx::TricksterGrenadeLauncherMarker);
	VfxsMap.Add("PA_SpyMuzzle", EVfx::SpyMuzzle);
	VfxsMap.Add("PA_SpyBulletTrail", EVfx::SpyBulletTrail);
	VfxsMap.Add("PA_SpyBulletImpact", EVfx::SpyBulletImpact);
	VfxsMap.Add("PA_SpyFpsBulletTrail", EVfx::SpyFpsBulletTrail);
	VfxsMap.Add("PA_TankMuzzle", EVfx::TankMuzzle);
	VfxsMap.Add("PA_TankBulletTrail", EVfx::TankBulletTrail);
	VfxsMap.Add("PA_TankBulletImpact", EVfx::TankBulletImpact);
	VfxsMap.Add("PA_TankFpsBulletTrail", EVfx::TankFpsBulletTrail);
	VfxsMap.Add("PA_GuardianSpawn", EVfx::GuardianSpawn);
	VfxsMap.Add("PA_GuardianBulletTrail", EVfx::GuardianBulletTrail);
	VfxsMap.Add("PA_GuardianBulletImpact", EVfx::GuardianBulletImpact);
	VfxsMap.Add("PA_GuardianMuzzle", EVfx::GuardianMuzzle);
	VfxsMap.Add("PA_GuardianDeathElectrification", EVfx::GuardianDeathElectrification);
	VfxsMap.Add("PA_GuardianVectorBlastUpgrade", EVfx::GuardianVectorBlastUpgrade);
	VfxsMap.Add("PA_AIGuardianShield", EVfx::AI_GuardianShield);
	VfxsMap.Add("PA_PlayerGuardianShield", EVfx::PlayerGuardianShield);
	VfxsMap.Add("PA_MinionBossBulletTrail", EVfx::MinionBossBulletTrail);
	VfxsMap.Add("PA_MinionBossMuzzle", EVfx::MinionBossMuzzle);
	VfxsMap.Add("PA_MinionBossStompCrack", EVfx::MinionBossStompCrack);
	VfxsMap.Add("PA_MinionBossLaserBall", EVfx::MinionBossLaserBall);
	VfxsMap.Add("PA_MinionBossLaser", EVfx::MinionBossLaser);
	VfxsMap.Add("PA_MinionBossDeathExplosion", EVfx::MinionBossDeathExplosion);
	VfxsMap.Add("PA_MinionBossDeathElectrification", EVfx::MinionBossDeathElectrification);
	VfxsMap.Add("PA_MinionBossSpawn", EVfx::MinionBossSpawn);
	VfxsMap.Add("PA_PlayerStomp", EVfx::PlayerStomp);
	VfxsMap.Add("PA_UsbAura", EVfx::UsbAura);
	VfxsMap.Add("PA_ArtifactAura", EVfx::ArtifactAura);
	VfxsMap.Add("PA_CoinAura", EVfx::CoinAura);
	VfxsMap.Add("PA_EmpExplosion", EVfx::EmpExplosion);
	VfxsMap.Add("PA_PlayerStomp", EVfx::PlayerStomp);
	VfxsMap.Add("PA_SecretRewardPopUp", EVfx::SecretRewardPopUp);
	VfxsMap.Add("PA_EmpAffected", EVfx::EmpAffected);
	VfxsMap.Add("PA_PoisonSlowAffected", EVfx::PoisonSlowAffected);
	VfxsMap.Add("PA_PossessionViable", EVfx::HijackViable);
	VfxsMap.Add("PA_TutorialBotLandOnHead", EVfx::TutorialBotLandOnHead);
	VfxsMap.Add("PA_HitVent", EVfx::HitVent);
	VfxsMap.Add("PA_CrashVent", EVfx::CrashVent);
	VfxsMap.Add("PA_ComputerElectrification", EVfx::ComputerElectrification);
	VfxsMap.Add("PA_DoorBreak", EVfx::DoorBreak);
	VfxsMap.Add("PA_CloneBullet", EVfx::CloneBullet);
	VfxsMap.Add("PA_CloneBulletImpact", EVfx::CloneBulletImpact);
	VfxsMap.Add("PA_CloneSpawn", EVfx::CloneSpawn);
	VfxsMap.Add("PA_VectorBlastTrail", EVfx::VectorBlastTrail);
	VfxsMap.Add("PA_VectorBlastImpact", EVfx::VectorBlastImpact);
	VfxsMap.Add("PA_VectorBlastMuzzle", EVfx::VectorBlastMuzzle);
	VfxsMap.Add("PA_WardenBlasterProjectile", EVfx::WardenBlasterProjectile);
	VfxsMap.Add("PA_WardenBeamShot", EVfx::WardenBeamShot);
	VfxsMap.Add("PA_WardenSpawn", EVfx::WardenSpawn);
	VfxsMap.Add("PA_WardenDeathElectrification", EVfx::WardenDeathElectrification);
	VfxsMap.Add("PA_RunEffect", EVfx::RunEffect);
	VfxsMap.Add("PA_LevelUpEffect", EVfx::LevelUpEffect);
	VfxsMap.Add("PA_BreacherBarrelExplosion", EVfx::BreacherBarrelExplosion);
	VfxsMap.Add("PA_BreacherMuzzle", EVfx::BreacherMuzzle);
	VfxsMap.Add("PA_BreacherBulletTrail", EVfx::BreacherBulletTrail);
	VfxsMap.Add("PA_BreacherBulletImpact", EVfx::BreacherBulletImpact);
	FillContainers<EVfx, FVfxContainer, UPA_VfxContainer>(VfxsMap, VfxContainers, "Vfx");
	VfxsMap.Empty();
}

void UPrimaryDataAssetsManager::SetAndPlaySoundCallback(const FSoundContainer& SoundData, const TObjectPtr<UAudioComponent>& InAudioComp, TFunction<bool()> InCallback, const bool bIsPlayer)
{
	USoundBase* SoundPtr = SoundData.GetSoundContainer()->Sound.Get();
	InAudioComp->SetSound(SoundPtr);
	if (InCallback && !InCallback()) return;
	if (SoundData.GetSoundContainer()->bIsCharacterTypeDependant) InAudioComp->SetFloatParameter("Volume", bIsPlayer ? SoundData.GetSoundContainer()->PlayerVolume : SoundData.GetSoundContainer()->EnemyVolume);
	InAudioComp->Play();
}

void UPrimaryDataAssetsManager::SetupVfx(const FVfxContainer& InVfxData, TFunction<bool()> InCallbackOfDeactivation, const TObjectPtr<UNiagaraComponent>& InVfxComponent, const bool bDeactivate, const EVariation InVariationName, const TMap<FName, FNiagaraAttribute>& InAttributes)
{
	UpdateVfxAttributes(InVfxComponent, InAttributes);
	if (!InVfxComponent) return;
	if (bDeactivate || (InCallbackOfDeactivation && !InCallbackOfDeactivation())) InVfxComponent->DeactivateImmediate();
	TMap<FName, FNiagaraUserParam>* Params = InVfxData.GetVfxContainer()->GetParams(InVariationName);
	if (!Params) return;
	FNiagaraUserRedirectionParameterStore& NiagaraParams = InVfxComponent->GetOverrideParameters();
	for (const TPair<FName, FNiagaraUserParam>& Param : *Params)
	{
		switch (Param.Value.ParamType)
		{
			case EUserParamType::Bool: InVfxComponent->SetBoolParameter(Param.Key, Param.Value.bBoolValue); break;
			case EUserParamType::Int: InVfxComponent->SetIntParameter(Param.Key, Param.Value.IntValue); break;
			case EUserParamType::Float: InVfxComponent->SetFloatParameter(Param.Key, Param.Value.FloatValue); break;
			case EUserParamType::Vector: InVfxComponent->SetVectorParameter(Param.Key, Param.Value.VectorValue); break;
			case EUserParamType::Color: InVfxComponent->SetColorParameter(Param.Key, Param.Value.ColorValue); break;
			case EUserParamType::Material:
				FNiagaraVariable NiagaraParamName(UMaterialInterface::StaticClass(), Param.Key);
				TObjectPtr<UMaterialInstanceDynamic> MI_Dynamic = Cast<UMaterialInstanceDynamic>(NiagaraParams.GetUObject(NiagaraParamName));
				if (!MI_Dynamic)
				{
					MI_Dynamic = UMaterialInstanceDynamic::Create(Param.Value.MaterialValue, InVfxComponent.Get());
					InVfxComponent->SetVariableMaterial(Param.Key, MI_Dynamic);
				}
				TMap<FName, FDynamicMaterialParams> MaterialParams = Param.Value.MaterialParams;
				for (const TPair<FName, FDynamicMaterialParams> MaterialParam : MaterialParams) MI_Dynamic->SetScalarParameterValue(MaterialParam.Key, MaterialParam.Value.FloatValue);
		}
	}
}

void UPrimaryDataAssetsManager::AsyncLoadEnemy(const ECharacterType InCharacter, const EGameMode InGameMode, TFunction<void()> InCallBack, TFunction<bool()> InSpawnCallback)
{
	if (!GetCanAsyncLoadHappen()) return;
	const FCharacterContainer& CharacterContainer = GetCharacterData(InCharacter);
	TArray<FSoftObjectPath> PathsToLoad;
	const TObjectPtr<UPA_CharacterContainer> CharContainer = CharacterContainer.GetCharacterContainer();
	if (!IsValid(CharContainer))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid CharacterContainer for character %s"), *UEnum::GetValueAsString(InCharacter));
		return;
	}
	if (!CharContainer->CharacterClass.IsNull()) PathsToLoad.Add(CharContainer->CharacterClass.ToSoftObjectPath());
	else UE_LOG(LogTemp, Warning, TEXT("CharacterClass is invalid for character %s"), *UEnum::GetValueAsString(InCharacter));
	if (!CharContainer->CharacterMesh.IsNull()) PathsToLoad.Add(CharContainer->CharacterMesh.ToSoftObjectPath());
	else UE_LOG(LogTemp, Warning, TEXT("CharacterMesh is invalid for character %s"), *UEnum::GetValueAsString(InCharacter));
	EWeapon Weapon = GetWeaponByCharacter(InCharacter);
	const TObjectPtr<UPA_WeaponContainer> WeaponContainer = GetWeaponData(Weapon);
	if (!IsValid(WeaponContainer))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid WeaponContainer for weapon %s"), *UEnum::GetValueAsString(Weapon));
		return;
	}
	if (!WeaponContainer->WeaponClass.IsNull()) PathsToLoad.Add(WeaponContainer->WeaponClass.ToSoftObjectPath());
	else UE_LOG(LogTemp, Warning, TEXT("WeaponClass is invalid for weapon %s"), *UEnum::GetValueAsString(Weapon));
	if (!WeaponContainer->HandsMesh.IsNull()) PathsToLoad.Add(WeaponContainer->HandsMesh.ToSoftObjectPath());
	else UE_LOG(LogTemp, Warning, TEXT("HandsMesh is invalid for weapon %s"), *UEnum::GetValueAsString(Weapon));
	EWeapon SingularWeapon = GetSingularWeapon(Weapon);
	if (SingularWeapon != EWeapon::None)
	{
		const TObjectPtr<UPA_WeaponContainer> SingularWeaponContainer = GetWeaponData(SingularWeapon);
		if (IsValid(SingularWeaponContainer))
		{
			if (!SingularWeaponContainer->WeaponClass.IsNull()) PathsToLoad.Add(SingularWeaponContainer->WeaponClass.ToSoftObjectPath());
			else UE_LOG(LogTemp, Warning, TEXT("SingularWeaponClass is invalid for weapon %s"), *UEnum::GetValueAsString(SingularWeapon));
			if (!SingularWeaponContainer->WeaponMesh.IsNull()) PathsToLoad.Add(SingularWeaponContainer->WeaponMesh.ToSoftObjectPath());
			else UE_LOG(LogTemp, Warning, TEXT("SingularWeaponMesh is invalid for weapon %s"), *UEnum::GetValueAsString(SingularWeapon));
		}
	}
	else if (!WeaponContainer->WeaponMesh.IsNull()) PathsToLoad.Add(WeaponContainer->WeaponMesh.ToSoftObjectPath());
	else UE_LOG(LogTemp, Warning, TEXT("WeaponMesh is invalid for weapon %s"), *UEnum::GetValueAsString(Weapon));
	TArray<FSoftObjectPath> ValidPaths;
	for (const FSoftObjectPath& Path : PathsToLoad) if (!Path.GetAssetPathString().IsEmpty()) ValidPaths.Add(Path);
	if (ValidPaths.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid paths to async load for character %s"), *UEnum::GetValueAsString(InCharacter));
		return;
	}
	for (const FSoftObjectPath& Path : ValidPaths) UE_LOG(LogTemp, Warning, TEXT("Path: %s"), *Path.ToString());
	TWeakObjectPtr WeakThis(this);
	TWeakObjectPtr WeakWorld = GetWorld();
	TSharedPtr<FStreamableHandle> Handle = UAssetManager::GetStreamableManager().RequestAsyncLoad(ValidPaths, [this, WeakThis, WeakWorld, SingularWeapon, Weapon, InCharacter, InGameMode, InCallBack, InSpawnCallback]
	{
		if (!WeakThis.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("AsyncLoadEnemy finished but manager no longer exists"));
			return;
		}
		if (!WeakWorld.IsValid() || !WeakThis->IsAsyncAssetWorldValid(WeakWorld.Get()))
		{
			UE_LOG(LogTemp, Warning, TEXT("World no longer valid during AsyncLoadEnemy callback for character %s"), *UEnum::GetValueAsString(InCharacter));
			return;
		}
		if (InSpawnCallback)
		{
			if (!InSpawnCallback()) WeakThis->AsyncLoadEnemyCallback(SingularWeapon, Weapon, InCharacter, InGameMode, InCallBack);
		}
		else WeakThis->AsyncLoadEnemyCallback(SingularWeapon, Weapon, InCharacter, InGameMode, InCallBack);
	});
	AddAsyncHandle(Handle);
}

void UPrimaryDataAssetsManager::AsyncLoadEnemyCallback(const EWeapon InSingularWeapon, const EWeapon InWeapon, const ECharacterType InCharacter, const EGameMode InGameMode, TFunction<void()> InCallBack)
{
	const TObjectPtr<UWeaponManager> WeaponManager = GetGameInstance()->GetSubsystem<UWeaponManager>();
	if (!WeaponManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponManager subsystem is null."));
		if (InCallBack) InCallBack();
		return;
	}
	UObject* EnemyClass = GetCharacterData(InCharacter).GetCharacterContainer()->CharacterClass.ToSoftObjectPath().TryLoad();
	if (!EnemyClass) UE_LOG(LogTemp, Warning, TEXT("Failed to load character class for %s"), *UEnum::GetValueAsString(InCharacter));
	LoadedEnemies.Add(InCharacter, Cast<UClass>(EnemyClass));
	if (InSingularWeapon != EWeapon::None)
	{
		UObject* SingularWeaponClass = GetWeaponData(InSingularWeapon)->WeaponClass.ToSoftObjectPath().TryLoad();
		if (!SingularWeaponClass) { UE_LOG(LogTemp, Warning, TEXT("Failed to load singular weapon class %s"), *UEnum::GetValueAsString(InSingularWeapon)); }
		else WeaponManager->AddWeapon(InSingularWeapon, Cast<UClass>(SingularWeaponClass));
	}
	UObject* WeaponClass = GetWeaponData(InWeapon)->WeaponClass.ToSoftObjectPath().TryLoad();
	if (!WeaponClass) { UE_LOG(LogTemp, Warning, TEXT("Failed to load weapon class %s"), *UEnum::GetValueAsString(InWeapon)); }
	else WeaponManager->AddWeapon(InWeapon, Cast<UClass>(WeaponClass));
	SpawnEnemy(InCharacter, InGameMode, InCallBack);
}

void UPrimaryDataAssetsManager::SpawnEnemy(const ECharacterType InCharacter, const EGameMode InCurrentGameMode, TFunction<void()> InCallBack)
{
	FGameModeSpawnLocations* SpawnLocationsByGameMode = SpawnLocations.Find(InCurrentGameMode);
	if (!SpawnLocationsByGameMode) return;
	if (SpawnLocationsByGameMode->SpawnLocations.Num() > 0)
	{
		const int32 LastIndex = SpawnLocationsByGameMode->SpawnLocations.Num() - 1;
		for (int32 i = 0; i <= LastIndex; i++)
		{
			const int32 Index = FMath::RandRange(i, LastIndex);
			if (i != Index) SpawnLocationsByGameMode->SpawnLocations.Swap(i, Index);
		}
		for (const TObjectPtr<ASpawnLocation>& Location : SpawnLocationsByGameMode->SpawnLocations)
		{
			if (const TObjectPtr<AEnemy> Enemy = Location->SpawnEnemy(LoadedEnemies[InCharacter]))
			{
				Enemy->AI_SetupUpgradesAppliance();
				if (InCallBack) InCallBack();
				break;
			}
		}
	}
}

void UPrimaryDataAssetsManager::SpawnTutorialEnemy(const ECharacterType InCharacter, const EGameMode InGameMode) { RequestSpawnEnemy(InCharacter, InGameMode, nullptr, nullptr); }

void UPrimaryDataAssetsManager::AddSpawnableEnemiesToThePool()
{
	while (NextSpawnableEnemyIndex < SpawnableEnemiesByTotalAmount.Num() && SpawnableEnemiesByTotalAmount[NextSpawnableEnemyIndex].SpawnThreshold <= TotalSpawnedEnemies)
	{
		EnemyTypes.Add(SpawnableEnemiesByTotalAmount[NextSpawnableEnemyIndex].Character);
		NextSpawnableEnemyIndex++;
	}
}

void UPrimaryDataAssetsManager::ClearBeforeInit()
{
	TotalSpawnedEnemies = 0;
	NextSpawnableEnemyIndex = 0;
	EnemyTypes.Empty();
	bCanAsyncLoadHappen = true;
	bIsCloneSpawnInProcess = false;
	for (TPair<ESound, FSoundContainer>& SoundContainer : SoundContainers) SoundContainer.Value.ResetLoadVariables();
	for (TPair<EVfx, FVfxContainer>& VfxContainer : VfxContainers) VfxContainer.Value.ResetLoadVariables();
	SpawnLocations.Empty();
	LoadedEnemies.Empty();
}

void UPrimaryDataAssetsManager::LoadVfx(FVfxContainer& InVfxData, const EVfx InVfxName, TFunction<bool()> InCallbackOfDeactivation, TObjectPtr<UNiagaraComponent>* InVfxComponent, const TObjectPtr<USceneComponent>& InAttachTo, const FName InSocketName, const bool bDeactivate, const FVector& InLocation, const FRotator& InRotation, const EAttachLocation::Type InAttachType, const bool bAutoDestroy, const EVariation InVariationName, const TMap<FName, FNiagaraAttribute>& InAttributes)
{
	InVfxData.AddVfxLoadedCallback([this, InVfxData, InCallbackOfDeactivation, InVfxComponent, InVfxName, InAttachTo, InSocketName, bDeactivate, InLocation, InRotation, InAttachType, bAutoDestroy, InVariationName, InAttributes]
	{
		if (InVfxComponent)
		{
			*InVfxComponent = AttachAndReceiveVfx(InVfxName, InAttachTo, InSocketName, InLocation, InRotation, InAttachType, bAutoDestroy);
			SetupVfx(InVfxData, InCallbackOfDeactivation, *InVfxComponent, bDeactivate, InVariationName, InAttributes);
		}
		else SetupVfx(InVfxData, InCallbackOfDeactivation, AttachAndReceiveVfx(InVfxName, InAttachTo, InSocketName, InLocation, InRotation, InAttachType, bAutoDestroy), bDeactivate, InVariationName, InAttributes);
	});
	RequestVfxLoad(InVfxData);
}

void UPrimaryDataAssetsManager::LoadVfx(FVfxContainer& InVfxData, const EVfx InVfxName, TFunction<bool()> InCallbackOfDeactivation, const FVector& InLocation, const FRotator& InRotation, const EVariation InVariationName, const TMap<FName, FNiagaraAttribute>& InAttributes)
{
	InVfxData.AddVfxLoadedCallback([this, InVfxData, InCallbackOfDeactivation, InVfxName, InLocation, InRotation, InVariationName, InAttributes]
	{
		SetupVfx(InVfxData, InCallbackOfDeactivation, SpawnAndReceiveVfx(InVfxName, InLocation, InRotation),  false, InVariationName, InAttributes);
	});
	RequestVfxLoad(InVfxData);
}

void UPrimaryDataAssetsManager::RequestVfxLoad(FVfxContainer& InVfxData)
{
	if (!GetCanAsyncLoadHappen()) return;
	TWeakObjectPtr WeakWorld = GetWorld();
	FVfxContainer* VfxContainer = &InVfxData;
	TSharedPtr<FStreamableHandle> Handle = UAssetManager::GetStreamableManager().RequestAsyncLoad(InVfxData.GetVfxContainer()->GetVfxEffect().ToSoftObjectPath(), [this, WeakWorld, VfxContainer]
	{
		if (!WeakWorld.IsValid() || !IsAsyncAssetWorldValid(WeakWorld.Get()) || !VfxContainer) return;
		VfxContainer->SetIsLoaded(true);
		VfxContainer->TriggerVfxsCallbacks();
	});
	AddAsyncHandle(Handle);
}

template<typename EnumType, typename ContainerType, typename AssetType>
void UPrimaryDataAssetsManager::FillContainers(TMap<FName, EnumType>& InMap, TMap<EnumType, ContainerType>& InContainers, const FName InType)
{
	TArray<FAssetData> Assets;
	UAssetManager::Get().GetPrimaryAssetDataList(FPrimaryAssetType(InType), Assets);
	for (int i = 0; i < Assets.Num(); i++) InContainers.Add(InMap[Assets[i].AssetName], ContainerType(Cast<AssetType>(Assets[i].GetAsset())));
	Assets.Empty();
}

template<typename EnumType, typename ContainerType>
void UPrimaryDataAssetsManager::FillContainers(TMap<FName, EnumType>& InMap, TMap<EnumType, TObjectPtr<ContainerType>>& InContainers, const FName InType)
{
	TArray<FAssetData> Assets;
	UAssetManager::Get().GetPrimaryAssetDataList(FPrimaryAssetType(InType), Assets);
	for (int i = 0; i < Assets.Num(); i++) InContainers.Add(InMap[Assets[i].AssetName], Cast<ContainerType>(Assets[i].GetAsset()));
	Assets.Empty();
}

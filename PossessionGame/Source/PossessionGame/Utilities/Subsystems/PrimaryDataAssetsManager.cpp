#include "PrimaryDataAssetsManager.h"
#include "DelegateManager.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "WeaponManager.h"
#include "Components/AudioComponent.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_CharacterContainer.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_SoundContainer.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_WeaponContainer.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_VfxContainer.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/Enums/EVfxType.h"
#include "PossessionGame/Projectiles/Projectile.h"
#include "PossessionGame/Utilities/Data Utilities/FNiagaraAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/FNiagaraVariationData.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/SpawnLocation.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"

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

TObjectPtr<UPA_SoundContainer>& UPrimaryDataAssetsManager::GetSoundData(ESound SoundName) { return SoundContainers[SoundName]; }

TObjectPtr<UPA_CharacterContainer>& UPrimaryDataAssetsManager::GetCharacterData(ECharacterType CharacterName) { return CharacterContainers[CharacterName]; }

TObjectPtr<UPA_VfxContainer>& UPrimaryDataAssetsManager::GetVfxData(EVfx VfxName) { return VfxContainers[VfxName]; }

TObjectPtr<AEnemy> UPrimaryDataAssetsManager::SpawnVariationReplacement(const ECharacterType InCharacter)
{
	return SpawnLocations[EGameMode::None].SpawnLocations[0]->SpawnEnemy(LoadedEnemies[InCharacter]);
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
		{FPoolObjectPairWrapper({ EPoolObject::MinionProjectile, EVfxType::Muzzle }), EVfx::MinionMuzzle },
		{FPoolObjectPairWrapper({ EPoolObject::MinionProjectile, EVfxType::Trail }), EVfx::MinionBulletTrail },
		{FPoolObjectPairWrapper({ EPoolObject::MinionProjectile, EVfxType::Impact }), EVfx::MinionBulletImpact },
		{FPoolObjectPairWrapper({ EPoolObject::TricksterProjectile, EVfxType::Muzzle }), EVfx::TricksterMuzzle },
		{FPoolObjectPairWrapper({ EPoolObject::TricksterProjectile, EVfxType::FpsTrail }), EVfx::TricksterFpsBulletTrail },
		{FPoolObjectPairWrapper({ EPoolObject::TricksterProjectile, EVfxType::Trail }), EVfx::TricksterBulletTrail },
		{FPoolObjectPairWrapper({ EPoolObject::TricksterPoisonProjectile, EVfxType::Impact }), EVfx::TricksterPoisonImpact },
		{FPoolObjectPairWrapper({ EPoolObject::TricksterPoisonProjectile, EVfxType::SecondaryImpact }), EVfx::TricksterPoison },
		{FPoolObjectPairWrapper({ EPoolObject::TricksterEmpProjectile, EVfxType::Impact }), EVfx::TricksterEmpImpact },
		{FPoolObjectPairWrapper({ EPoolObject::SpyProjectile, EVfxType::Trail }), EVfx::SpyBulletTrail },
		{FPoolObjectPairWrapper({ EPoolObject::SpyProjectile, EVfxType::Impact }), EVfx::SpyBulletImpact },
		{FPoolObjectPairWrapper({ EPoolObject::SpyProjectile, EVfxType::Muzzle }), EVfx::SpyMuzzle },
		{FPoolObjectPairWrapper({ EPoolObject::SpyProjectile, EVfxType::FpsTrail }), EVfx::SpyFpsBulletTrail },
		{FPoolObjectPairWrapper({ EPoolObject::TankProjectile, EVfxType::Trail }), EVfx::TankBulletTrail },
		{FPoolObjectPairWrapper({ EPoolObject::TankProjectile, EVfxType::Impact }), EVfx::TankBulletImpact },
		{FPoolObjectPairWrapper({ EPoolObject::TankProjectile, EVfxType::Muzzle }), EVfx::TankMuzzle },
		{FPoolObjectPairWrapper({ EPoolObject::TankProjectile, EVfxType::FpsTrail }), EVfx::TankFpsBulletTrail },
		{FPoolObjectPairWrapper({ EPoolObject::GuardianProjectile, EVfxType::FpsTrail }), EVfx::GuardianFpsBulletTrail },
		{FPoolObjectPairWrapper({ EPoolObject::GuardianProjectile, EVfxType::Muzzle }), EVfx::GuardianMuzzle },
		{FPoolObjectPairWrapper({ EPoolObject::GuardianProjectile, EVfxType::Trail }), EVfx::GuardianBulletTrail },
		{FPoolObjectPairWrapper({ EPoolObject::MinionBossProjectile, EVfxType::Trail }), EVfx::MinionBossBulletTrail },
		{FPoolObjectPairWrapper({ EPoolObject::MinionBossProjectile, EVfxType::Muzzle }), EVfx::MinionBossMuzzle }
	};
	CharacterVfxCombinationsMap =
	{
		{FCharacterPairWrapper({ ECharacterType::Guardian, EVfxType::Spawn }), EVfx::GuardianSpawn },
		{FCharacterPairWrapper({ ECharacterType::Minion, EVfxType::Spawn }), EVfx::MinionSpawn }
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

void UPrimaryDataAssetsManager::ConfigureSound(TObjectPtr<UAudioComponent> InAudioComp, TFunction<bool()> InCallback, ESound InSoundName)
{
	if (GetCanAsyncLoadHappen())
	{
		const FSoftObjectPath PathToLoad = GetSoundData(InSoundName)->Sound.ToSoftObjectPath();
		TSharedPtr<FStreamableHandle> Handle = UAssetManager::GetStreamableManager().RequestAsyncLoad(PathToLoad, [this, InAudioComp, InCallback, InSoundName]
		{
			if (IsGameRunning()) SetAndPlaySoundCallback(InAudioComp, InCallback, InSoundName);
		});
		AddAsyncHandle(Handle);
	}
}

void UPrimaryDataAssetsManager::PlaySound2D(const ESound InSoundName, const bool bStoreSoundRef)
{
	if (GetCanAsyncLoadHappen())
	{
		const FSoftObjectPath PathToLoad = GetSoundData(InSoundName)->Sound.ToSoftObjectPath();
		TSharedPtr<FStreamableHandle> Handle = UAssetManager::GetStreamableManager().RequestAsyncLoad(PathToLoad, [this, InSoundName, bStoreSoundRef]
		{
			if (IsGameRunning())
			{
				if (bStoreSoundRef) StoredSounds.Add(InSoundName, UGameplayStatics::SpawnSound2D(GetWorld(), GetSoundData(InSoundName)->Sound.Get()));
				else UGameplayStatics::PlaySound2D(GetWorld(), GetSoundData(InSoundName)->Sound.Get());
			}
		});
		AddAsyncHandle(Handle);
	}
}

void UPrimaryDataAssetsManager::RemoveStoredSound(const ESound InSoundName)
{
	if (StoredSounds.Contains(InSoundName))
	{
		StoredSounds[InSoundName]->Stop();
		StoredSounds.Remove(InSoundName);
	}
}

void UPrimaryDataAssetsManager::AttachVfx(const EVfx InVfxName, TFunction<bool()> InCallback, const FNiagaraVariationData& InNiagaraVariationData, TObjectPtr<UNiagaraComponent>* InVfxComponent, const TObjectPtr<USceneComponent>& InAttachTo, const FName InSocketName, const bool bDeactivate, const TMap<FName, FNiagaraAttribute>& InAttributes, const FVector& InLocation, const FRotator& InRotation, const EAttachLocation::Type InAttachType, const bool bAutoDestroy)
{
	if (InVfxName == EVfx::None) return;
	if (GetVfxData(InVfxName)->GetVfxEffect().IsValid())
	{
		if (InVfxComponent)
		{
			*InVfxComponent = AttachAndReceiveVfx(InVfxName, InAttachTo, InSocketName, InLocation, InRotation, InAttachType, bAutoDestroy);
			SetupVfx(InCallback, InNiagaraVariationData, *InVfxComponent, InAttributes, bDeactivate);
		}
		else SetupVfx(InCallback, InNiagaraVariationData, AttachAndReceiveVfx(InVfxName, InAttachTo, InSocketName, InLocation, InRotation, InAttachType, bAutoDestroy), InAttributes, bDeactivate);
	}
	else GetVfxData(InVfxName)->LoadVfx(GetWorld(), [this, InCallback, InNiagaraVariationData, InVfxComponent, InVfxName, InAttachTo, InAttributes, InSocketName, bDeactivate, InLocation, InRotation, InAttachType, bAutoDestroy]
	{
		if (!IsGameRunning()) return;
		if (InVfxComponent)
		{
			*InVfxComponent = AttachAndReceiveVfx(InVfxName, InAttachTo, InSocketName, InLocation, InRotation, InAttachType, bAutoDestroy);
			SetupVfx(InCallback, InNiagaraVariationData, *InVfxComponent, InAttributes, bDeactivate);
		}
		else SetupVfx(InCallback, InNiagaraVariationData, AttachAndReceiveVfx(InVfxName, InAttachTo, InSocketName, InLocation, InRotation, InAttachType, bAutoDestroy), InAttributes, bDeactivate);
	});
}

void UPrimaryDataAssetsManager::SpawnVfx(const EVfx InVfxName, TFunction<bool()> InCallback, const FVector& InLocation, const FRotator& InRotation, const FNiagaraVariationData& InNiagaraVariationData, const TMap<FName, FNiagaraAttribute>& InAttributes)
{
	if (InVfxName == EVfx::None) return;
	if (GetVfxData(InVfxName)->GetVfxEffect().IsValid())
	{
		SetupVfx(InCallback, InNiagaraVariationData, SpawnAndReceiveVfx(InVfxName, InLocation, InRotation), InAttributes);
	}
	else GetVfxData(InVfxName)->LoadVfx(GetWorld(), [this, InCallback, InNiagaraVariationData, InVfxName, InAttributes, InLocation, InRotation]
	{
		if (!IsGameRunning()) return;
		SetupVfx(InCallback, InNiagaraVariationData, SpawnAndReceiveVfx(InVfxName, InLocation, InRotation), InAttributes);
	});
}

void UPrimaryDataAssetsManager::ApplyAttributesToVfxComponent(const TMap<FName, FNiagaraAttribute>& InAttributes, const TObjectPtr<UNiagaraComponent>& Component)
{
	for (const TPair<FName, FNiagaraAttribute> Attribute : InAttributes)
	{
		if (!Attribute.Value.bIsReplacingValue) // Only float works currently, if there will be need for FVector / FLinearColor changes will occur.
		{
			if (FMath::IsNearlyEqual(Attribute.Value.GetFloat(), 0.f, 0.001f)) continue;
			const float ComponentAttributeValue = GetVfxAttributeValue(Component, Attribute.Key);
			const float AttributeValue = ComponentAttributeValue + ComponentAttributeValue * (Attribute.Value.GetFloat() * 0.01f);
			Component->SetVariableFloat(Attribute.Key, AttributeValue);
	//		if (Attribute.Value.IsFloat()) 
		}
		else
		{
			if (Attribute.Value.IsFloat()) Component->SetVariableFloat(Attribute.Key, Attribute.Value.GetFloat());
			else if (Attribute.Value.IsVector()) Component->SetVectorParameter(Attribute.Key, Attribute.Value.GetVector());
			else if (Attribute.Value.IsColor()) Component->SetColorParameter(Attribute.Key, Attribute.Value.GetColor());
			else if (Attribute.Value.IsMaterial()) Component->SetVariableMaterial(Attribute.Key, Attribute.Value.GetMaterial());
		}
	}
}

void UPrimaryDataAssetsManager::ChangeVfxVariation(const TObjectPtr<UNiagaraComponent>& InVfxComponent, const FNiagaraVariationData& InNiagaraVariationData)
{
	if (!InVfxComponent) return;
	TArray<FNiagaraVariable> UserParameters;
	InVfxComponent->GetOverrideParameters().GetUserParameters(UserParameters);
	for (const FNiagaraVariable& Param : UserParameters)
	{
		if (Param.GetType() != FNiagaraTypeDefinition(UMaterialInterface::StaticClass())) continue;
		if (TObjectPtr<UObject> MaterialObj = InVfxComponent->GetOverrideParameters().GetUObject(Param))
		{
			if (TObjectPtr<UMaterialInstanceDynamic> MI_Dynamic = Cast<UMaterialInstanceDynamic>(MaterialObj))
			{
				MI_Dynamic->SetScalarParameterValue("Hue", InNiagaraVariationData.Hue);
			}
			else if (TObjectPtr<UMaterialInterface> MI = Cast<UMaterialInterface>(MaterialObj))
			{
				MI_Dynamic = UMaterialInstanceDynamic::Create(MI, this);
				InVfxComponent->SetVariableMaterial(Param.GetName(), MI_Dynamic);
				MI_Dynamic->SetScalarParameterValue("Hue", InNiagaraVariationData.Hue);
			}
		}
	}
}

void UPrimaryDataAssetsManager::RequestSpawnEnemy(ECharacterType InCharacter, const EGameMode InGameMode, const bool bCanEvolve, TFunction<void()> InCallBack)
{
	if (!bIsSpawnInProcess)
	{
		if (InCharacter == ECharacterType::Random) InCharacter = GetRandomCharacterToSpawn();
		!LoadedEnemies.Contains(InCharacter) ? AsyncLoadEnemy(InCharacter, InGameMode, bCanEvolve, InCallBack) : SpawnEnemy(InCharacter, InGameMode, bCanEvolve, InCallBack);
		TotalSpawnedEnemies++;
		AddSpawnableEnemiesToThePool();
	}
}

void UPrimaryDataAssetsManager::Init(const bool bIsTutorialFinished, const TSubclassOf<AMarker>& InMarkerClass, const TObjectPtr<UDelegateManager>& InDelegateManager, const TArray<FSpawnableEnemyByTotalAmount>& InSpawnableEnemiesByTotalAmount)
{
	MarkerClass = InMarkerClass;
	bTutorialIsFinished = bIsTutorialFinished;
	SpawnableEnemiesByTotalAmount = InSpawnableEnemiesByTotalAmount;
	//RequestSpawnEnemy(ECharacterType::Trickster, EGameMode::TutorialMode1, false, nullptr);
	if (SpawnableEnemiesByTotalAmount.Num() < 1) EnemyTypes.Add(ECharacterType::Ranger);
	else AddSpawnableEnemiesToThePool();
	DelegateManager = InDelegateManager;
	if (!bTutorialIsFinished) DelegateManager->SpawnTutorialEnemy.BindUObject(this, &UPrimaryDataAssetsManager::SpawnTutorialEnemy);
}

void UPrimaryDataAssetsManager::SetupSpawnLocations()
{
	SpawnLocations.Empty();
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnLocation::StaticClass(), Actors);
	for (TObjectPtr<AActor> Actor : Actors)
	{
		TObjectPtr<ASpawnLocation> SpawnLocation = Cast<ASpawnLocation>(Actor);
		if (!SpawnLocations.Contains(SpawnLocation->GetGameMode())) SpawnLocations.Add(SpawnLocation->GetGameMode());
		SpawnLocations[SpawnLocation->GetGameMode()].SpawnLocations.Add(SpawnLocation);
	}
	SpawnTutorialEnemy(ECharacterType::Guardian, EGameMode::TutorialMode1);
}

bool UPrimaryDataAssetsManager::IsGameRunning()
{
	if (!IsValid(this)) return false;
	const TObjectPtr<UWorld> World = GetWorld();
	return IsValid(World) && !World->HasAnyFlags(RF_BeginDestroyed | RF_FinishDestroyed);
}

EPoolObject UPrimaryDataAssetsManager::GetProjectileNameByCharacterName(ECharacterType InCharacterType)
{
	switch (InCharacterType)
	{
		case ECharacterType::Ranger: return EPoolObject::RangerProjectile;
		case ECharacterType::Minion: return EPoolObject::MinionProjectile;
		case ECharacterType::Trickster: return EPoolObject::TricksterProjectile;
		case ECharacterType::Spy: return EPoolObject::SpyProjectile;
		case ECharacterType::Tank: return EPoolObject::TankProjectile;
		case ECharacterType::Guardian: return EPoolObject::GuardianProjectile;
		case ECharacterType::MinionBoss: return EPoolObject::MinionBossProjectile;
		default: return EPoolObject::None;
	}
}

TObjectPtr<UNiagaraComponent> UPrimaryDataAssetsManager::AttachAndReceiveVfx(const EVfx InVfxName, const TObjectPtr<USceneComponent>& InAttachTo, const FName InSocketName, const FVector& InLocation, const FRotator& InRotation,  const EAttachLocation::Type InAttachType,  const bool bAutoDestroy)
{
	return UNiagaraFunctionLibrary::SpawnSystemAttached(GetVfxData(InVfxName)->GetVfxEffect().Get(), InAttachTo, InSocketName, InLocation, InRotation, InAttachType, bAutoDestroy);
}

TObjectPtr<UNiagaraComponent> UPrimaryDataAssetsManager::SpawnAndReceiveVfx(const EVfx InVfxName, const FVector& InLocation, const FRotator& InRotation)
{
	return UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, GetVfxData(InVfxName)->GetVfxEffect().Get(), InLocation, InRotation);
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
		case ECharacterType::Samurai: return EWeapon::SamuraiWeapon;
		case ECharacterType::Guardian: return EWeapon::GuardianWeapon;
		case ECharacterType::MinionBoss: return EWeapon::MinionBossHandWeapon;
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
	SoundMap.Add("PA_OnPossession", ESound::OnPossession);
	SoundMap.Add("PA_ReadyToPossess", ESound::ReadyToPossess);
	SoundMap.Add("PA_TakeDamageLayer1", ESound::TakeDamageLayer1);
	SoundMap.Add("PA_TakeDamageLayer2", ESound::TakeDamageLayer2);
	SoundMap.Add("PA_CriticalDamageBeep", ESound::CriticalDamageBeep);
	SoundMap.Add("PA_CriticalHealthEnter", ESound::CriticalHealthEnter);
	SoundMap.Add("PA_PlayerDeath", ESound::PlayerDeath);
	SoundMap.Add("PA_RangerOnPossession", ESound::RangerOnPossession);
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
	SoundMap.Add("PA_TricksterOnPossession", ESound::TricksterOnPossession);
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
	SoundMap.Add("PA_DroneBuzz", ESound::DroneBuzz);
	SoundMap.Add("PA_DialogueStart", ESound::DialogueStart);
	SoundMap.Add("PA_RestSoundtrack", ESound::RestSoundtrack);
	SoundMap.Add("PA_BattleModeSoundtrack", ESound::BattleModeSoundtrack);
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
	SoundMap.Add("PA_Dash", ESound::Dash);
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
	FillContainers(SoundMap, SoundContainers, "Sound");
	SoundMap.Empty();
}

void UPrimaryDataAssetsManager::SetupCharactersPrimaryAssets()
{
	TMap<FName, ECharacterType> CharactersMap;
	CharactersMap.Add("PA_Spy", ECharacterType::Spy);
	CharactersMap.Add("PA_Trickster", ECharacterType::Trickster);
	CharactersMap.Add("PA_Tank", ECharacterType::Tank);
	CharactersMap.Add("PA_Ranger", ECharacterType::Ranger);
	CharactersMap.Add("PA_Minion", ECharacterType::Minion);
	CharactersMap.Add("PA_Samurai", ECharacterType::Samurai);
	CharactersMap.Add("PA_Guardian", ECharacterType::Guardian);
	CharactersMap.Add("PA_MinionBoss", ECharacterType::MinionBoss);
	FillContainers(CharactersMap, CharacterContainers, "Character");
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
	VfxsMap.Add("PA_StopTimeAbility", EVfx::StopTimeAbility);
	VfxsMap.Add("PA_StopTimeAbilityReverse", EVfx::StopTimeAbilityReverse);
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
	VfxsMap.Add("PA_TricksterEmpImpact", EVfx::TricksterEmpImpact);
	VfxsMap.Add("PA_TricksterReload", EVfx::TricksterReload);
	VfxsMap.Add("PA_TricksterDeathElectrification", EVfx::TricksterDeathElectrification);
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
	VfxsMap.Add("PA_GuardianMuzzle", EVfx::GuardianMuzzle);
	VfxsMap.Add("PA_GuardianFpsBulletTrail", EVfx::GuardianFpsBulletTrail);
	VfxsMap.Add("PA_GuardianDeathElectrification", EVfx::GuardianDeathElectrification);
	VfxsMap.Add("PA_MinionBossBulletTrail", EVfx::MinionBossBulletTrail);
	VfxsMap.Add("PA_MinionBossMuzzle", EVfx::MinionBossMuzzle);
	VfxsMap.Add("PA_MinionBossStompCrack", EVfx::MinionBossStompCrack);
	VfxsMap.Add("PA_MinionBossLaserBall", EVfx::MinionBossLaserBall);
	VfxsMap.Add("PA_MinionBossLaser", EVfx::MinionBossLaser);
	VfxsMap.Add("PA_MinionBossDeathExplosion", EVfx::MinionBossDeathExplosion);
	VfxsMap.Add("PA_MinionBossDeathElectrification", EVfx::MinionBossDeathElectrification);
	VfxsMap.Add("PA_PlayerStomp", EVfx::PlayerStomp);
	VfxsMap.Add("PA_UsbAura", EVfx::UsbAura);
	VfxsMap.Add("PA_ArtifactAura", EVfx::ArtifactAura);
	VfxsMap.Add("PA_MutantAura", EVfx::MutantAura);
	VfxsMap.Add("PA_MutantExplosion", EVfx::MutantExplosion);
	VfxsMap.Add("PA_PlayerStomp", EVfx::PlayerStomp);
	VfxsMap.Add("PA_SecretRewardPopUp", EVfx::SecretRewardPopUp);
	VfxsMap.Add("PA_EmpAffected", EVfx::EmpAffected);
	VfxsMap.Add("PA_PossessionViable", EVfx::PossessionViable);
	FillContainers(VfxsMap, VfxContainers, "Vfx");
	VfxsMap.Empty();
}

void UPrimaryDataAssetsManager::SetAndPlaySoundCallback(const TObjectPtr<UAudioComponent>& InAudioComp, TFunction<bool()> InCallback, const ESound InSoundName)
{
	InAudioComp->SetSound(GetSoundData(InSoundName)->Sound.Get());
	if (InCallback && !InCallback()) return;
	InAudioComp->Play();
}

void UPrimaryDataAssetsManager::SetupVfx(TFunction<bool()> InCallback, const FNiagaraVariationData& InNiagaraVariationData, const TObjectPtr<UNiagaraComponent>& InVfxComponent, const TMap<FName, FNiagaraAttribute>& InAttributes, const bool bDeactivate)
{
	if (bDeactivate || (InCallback && !InCallback())) InVfxComponent->Deactivate();
	if (InNiagaraVariationData.bRequiredVfxChange) ChangeVfxVariation(InVfxComponent, InNiagaraVariationData);
	if (!InAttributes.IsEmpty()) ApplyAttributesToVfxComponent(InAttributes, InVfxComponent);
}

void UPrimaryDataAssetsManager::AsyncLoadEnemy(const ECharacterType InCharacter, const EGameMode InGameMode, const bool bCanEvolve, TFunction<void()> InCallBack)
{
	bIsSpawnInProcess = true;
	if (GetCanAsyncLoadHappen())
	{
		TArray<FSoftObjectPath> PathsToLoad;
		PathsToLoad.Add(GetCharacterData(InCharacter)->CharacterClass.ToSoftObjectPath());
		PathsToLoad.Add(GetCharacterData(InCharacter)->CharacterMesh.ToSoftObjectPath());
		EWeapon Weapon = GetWeaponByCharacter(InCharacter);
		PathsToLoad.Add(GetWeaponData(Weapon)->WeaponClass.ToSoftObjectPath());
		PathsToLoad.Add(GetWeaponData(Weapon)->HandsMesh.ToSoftObjectPath());
		EWeapon SingularWeapon = GetSingularWeapon(Weapon);
		if (SingularWeapon != EWeapon::None)
		{
			PathsToLoad.Add(GetWeaponData(SingularWeapon)->WeaponClass.ToSoftObjectPath());
			PathsToLoad.Add(GetWeaponData(SingularWeapon)->WeaponMesh.ToSoftObjectPath());
		}
		else PathsToLoad.Add(GetWeaponData(Weapon)->WeaponMesh.ToSoftObjectPath());
		TSharedPtr<FStreamableHandle> Handle = UAssetManager::GetStreamableManager().RequestAsyncLoad(PathsToLoad, [this, SingularWeapon, Weapon, InCharacter, InGameMode, bCanEvolve, InCallBack]
		{
			if (IsGameRunning()) AsyncLoadCallback(SingularWeapon, Weapon, InCharacter, InGameMode, bCanEvolve, InCallBack);
		});
		AddAsyncHandle(Handle);
	}
}

void UPrimaryDataAssetsManager::AsyncLoadCallback(const EWeapon InSingularWeapon, const EWeapon InWeapon, const ECharacterType InCharacter, const EGameMode InGameMode, const bool bCanEvolve, TFunction<void()> InCallBack)
{
	LoadedEnemies.Add(InCharacter, Cast<UClass>(GetCharacterData(InCharacter)->CharacterClass.ToSoftObjectPath().TryLoad()));
	if (InSingularWeapon != EWeapon::None)
	{
		GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->AddWeapon(InSingularWeapon, Cast<UClass>(GetWeaponData(InSingularWeapon)->WeaponClass.ToSoftObjectPath().TryLoad()));
	}
	GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->AddWeapon(InWeapon, Cast<UClass>(GetWeaponData(InWeapon)->WeaponClass.ToSoftObjectPath().TryLoad()));
	SpawnEnemy(InCharacter, InGameMode, bCanEvolve, InCallBack);
}

void UPrimaryDataAssetsManager::SpawnEnemy(const ECharacterType InCharacter, const EGameMode InCurrentGameMode, const bool bCanEvolve, TFunction<void()> InCallBack)
{
	if (!SpawnLocations.Find(InCurrentGameMode)) return;
	if (SpawnLocations[InCurrentGameMode].SpawnLocations.Num() > 0)
	{
		const int32 LastIndex = SpawnLocations[InCurrentGameMode].SpawnLocations.Num() - 1;
		for (int32 i = 0; i <= LastIndex; i++)
		{
			const int32 Index = FMath::RandRange(i, LastIndex);
			if (i != Index) SpawnLocations[InCurrentGameMode].SpawnLocations.Swap(i, Index);
		}
		for (const TObjectPtr<ASpawnLocation>& Location : SpawnLocations[InCurrentGameMode].SpawnLocations)
		{
			if (const TObjectPtr<AEnemy> Enemy = Location->SpawnEnemy(LoadedEnemies[InCharacter]))
			{
				if (bCanEvolve) Enemy->AI_TryEvolving();
				else Enemy->AI_SetupUpgradesAppliance();
				if (InCallBack) InCallBack();
				bIsSpawnInProcess = false;
				break;
			}
		}
	}
}

void UPrimaryDataAssetsManager::SpawnTutorialEnemy(const ECharacterType InCharacter, const EGameMode InGameMode) { RequestSpawnEnemy(InCharacter, InGameMode, false, nullptr); }

void UPrimaryDataAssetsManager::AddSpawnableEnemiesToThePool()
{
	while (NextSpawnableEnemyIndex < SpawnableEnemiesByTotalAmount.Num() && SpawnableEnemiesByTotalAmount[NextSpawnableEnemyIndex].SpawnThreshold <= TotalSpawnedEnemies)
	{
		EnemyTypes.Add(SpawnableEnemiesByTotalAmount[NextSpawnableEnemyIndex].Character);
		NextSpawnableEnemyIndex++;
	}
}

template<typename T, typename U>
void UPrimaryDataAssetsManager::FillContainers(TMap<FName, T>& Map, TMap<T, TObjectPtr<U>>& Containers, FName Type)
{
	TArray<FAssetData> Assets;
	UAssetManager::Get().GetPrimaryAssetDataList(FPrimaryAssetType(Type), Assets);
	for (int i = 0; i < Assets.Num(); i++) Containers.Add(Map[Assets[i].AssetName], Cast<U>(Assets[i].GetAsset()));
	Assets.Empty();
}

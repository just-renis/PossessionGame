#include "PrimaryDataAssetsManager.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Components/AudioComponent.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_CharacterContainer.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_SoundContainer.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_WeaponContainer.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_VfxContainer.h"
#include "PossessionGame/Enums/EVfxType.h"
#include "PossessionGame/Projectiles/Projectile.h"
#include "PossessionGame/Utilities/Data Utilities/FNiagaraAttribute.h"

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

EVfx UPrimaryDataAssetsManager::GetVfxName(EPoolObject ProjectileName, EVfxType VfxType)
{
	if (const EVfx* FoundVfx = VfxCombinationsMap.Find(FPairWrapper({ ProjectileName, VfxType }))) return *FoundVfx;
	return EVfx::None;
}

EVfx UPrimaryDataAssetsManager::GetVfxName(ECharacterType CharacterType, EVfxType VfxType)
{
	if (const EVfx* FoundVfx = VfxCombinationsMap.Find(FPairWrapper({ GetProjectileNameByCharacterName(CharacterType), VfxType }))) return *FoundVfx;
	return EVfx::None;
}

TObjectPtr<UPA_WeaponContainer>& UPrimaryDataAssetsManager::GetWeaponData(EWeapon WeaponName) { return WeaponContainers[WeaponName]; }

TObjectPtr<UPA_SoundContainer>& UPrimaryDataAssetsManager::GetSoundData(ESound SoundName) { return SoundContainers[SoundName]; }

TObjectPtr<UPA_CharacterContainer>& UPrimaryDataAssetsManager::GetCharacterData(ECharacterType CharacterName) { return CharacterContainers[CharacterName]; }

TObjectPtr<UPA_VfxContainer>& UPrimaryDataAssetsManager::GetVfxData(EVfx VfxName) { return VfxContainers[VfxName]; }

TObjectPtr<UNiagaraComponent> UPrimaryDataAssetsManager::AttachVfx(const EVfx InVfxName, const TObjectPtr<USceneComponent>& InAttachTo, const FName InSocketName, const FVector& InLocation, const FRotator& InRotation,  const EAttachLocation::Type InAttachType,  const bool bAutoDestroy)
{
	return UNiagaraFunctionLibrary::SpawnSystemAttached(GetVfxData(InVfxName)->GetVfxEffect().Get(), InAttachTo, InSocketName, InLocation, InRotation, InAttachType, bAutoDestroy);
}

TObjectPtr<UNiagaraComponent> UPrimaryDataAssetsManager::SpawnVfx(const EVfx InVfxName, const FVector& InLocation, const FRotator& InRotation)
{
	return UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, GetVfxData(InVfxName)->GetVfxEffect().Get(), InLocation, InRotation);
}

void UPrimaryDataAssetsManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	VfxCombinationsMap =
	{
		{ FPairWrapper({ EPoolObject::RangerProjectile, EVfxType::Trail }), EVfx::RangerBulletTrail },
		{ FPairWrapper({ EPoolObject::RangerProjectile, EVfxType::Impact }), EVfx::RangerBulletImpact },
		{ FPairWrapper({ EPoolObject::RangerProjectile, EVfxType::Muzzle }), EVfx::RangerMuzzle },
		{ FPairWrapper({ EPoolObject::RangerProjectile, EVfxType::FpsTrail }), EVfx::RangerFpsBulletTrail },
		{ FPairWrapper({ EPoolObject::RangerGrenadeLauncherProjectile, EVfxType::Impact }), EVfx::RangerGrenadeExplosion },
		{ FPairWrapper({ EPoolObject::MinionProjectile, EVfxType::Muzzle }), EVfx::MinionMuzzle },
		{ FPairWrapper({ EPoolObject::MinionProjectile, EVfxType::Trail }), EVfx::MinionBulletTrail },
		{ FPairWrapper({ EPoolObject::MinionProjectile, EVfxType::Impact }), EVfx::MinionBulletImpact },
		{ FPairWrapper({ EPoolObject::TricksterProjectile, EVfxType::Muzzle }), EVfx::TricksterMuzzle },
		{ FPairWrapper({ EPoolObject::TricksterProjectile, EVfxType::FpsTrail }), EVfx::TricksterFpsBulletTrail },
		{ FPairWrapper({ EPoolObject::TricksterProjectile, EVfxType::Impact }), EVfx::TricksterBulletImpact },
		{ FPairWrapper({ EPoolObject::TricksterProjectile, EVfxType::Trail }), EVfx::TricksterBulletTrail },
		{ FPairWrapper({ EPoolObject::TricksterPoisonProjectile, EVfxType::Impact }), EVfx::TricksterPoisonImpact },
		{ FPairWrapper({ EPoolObject::TricksterPoisonProjectile, EVfxType::SecondaryImpact }), EVfx::TricksterPoison },
		{ FPairWrapper({ EPoolObject::TricksterEmpProjectile, EVfxType::Impact }), EVfx::TricksterEmpImpact },
		{ FPairWrapper({ EPoolObject::SpyProjectile, EVfxType::Trail }), EVfx::SpyBulletTrail },
		{ FPairWrapper({ EPoolObject::SpyProjectile, EVfxType::Impact }), EVfx::SpyBulletImpact },
		{ FPairWrapper({ EPoolObject::SpyProjectile, EVfxType::Muzzle }), EVfx::SpyMuzzle },
		{ FPairWrapper({ EPoolObject::SpyProjectile, EVfxType::FpsTrail }), EVfx::SpyFpsBulletTrail },
		{ FPairWrapper({ EPoolObject::TankProjectile, EVfxType::Trail }), EVfx::TankBulletTrail },
		{ FPairWrapper({ EPoolObject::TankProjectile, EVfxType::Impact }), EVfx::TankBulletImpact },
		{ FPairWrapper({ EPoolObject::TankProjectile, EVfxType::Muzzle }), EVfx::TankMuzzle },
		{ FPairWrapper({ EPoolObject::TankProjectile, EVfxType::FpsTrail }), EVfx::TankFpsBulletTrail }
	};
	Init();
}

void UPrimaryDataAssetsManager::Init()
{
	SetupWeaponsPrimaryAssets();
	SetupCharactersPrimaryAssets();
	SetupSoundsPrimaryAssets();
	SetupVfxsPrimaryAssets();
}

void UPrimaryDataAssetsManager::DeleteLoadedHandles()
{
	for (int32 i = 0; i < AsyncHandles.Num(); i++)
	{
		if (AsyncHandles[i].Handle->HasLoadCompleted())
		{
			AsyncHandles[i].Handle->ReleaseHandle();
			AsyncHandles.RemoveAt(i);
		}
	}
	if (AsyncHandles.IsEmpty()) bCanAsyncLoadHappen = false;
}

void UPrimaryDataAssetsManager::ConfigureSound(TObjectPtr<UAudioComponent> InAudioComp, ESound InSoundName)
{
	if (GetCanAsyncLoadHappen())
	{
		const FSoftObjectPath PathToLoad = GetSoundData(InSoundName)->Sound.ToSoftObjectPath();
		TSharedPtr<FStreamableHandle> Handle = UAssetManager::GetStreamableManager().RequestAsyncLoad(PathToLoad, [this, InAudioComp, InSoundName]
		{
			if (IsGameRunning()) SetAndPlaySoundCallback(InAudioComp, InSoundName);
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

void UPrimaryDataAssetsManager::AttachOrLoadVfx(const EVfx InVfxName, TObjectPtr<UNiagaraComponent>* InVfxComponent, const TObjectPtr<USceneComponent>& InAttachTo, const FName InSocketName, const bool bDeactivate, const FVector& InLocation, const FRotator& InRotation, const EAttachLocation::Type InAttachType, const bool bAutoDestroy)
{
	if (GetVfxData(InVfxName)->GetVfxEffect().IsValid())
	{
		if (InVfxComponent) *InVfxComponent = AttachVfx(InVfxName, InAttachTo, InSocketName, InLocation, InRotation, InAttachType, bAutoDestroy);
		else AttachVfx(InVfxName, InAttachTo, InSocketName, InLocation, InRotation, InAttachType, bAutoDestroy);
	}
	else GetVfxData(InVfxName)->LoadVfx(GetWorld(), [this, InVfxComponent, InVfxName, InAttachTo, InSocketName, bDeactivate, InLocation, InRotation, InAttachType, bAutoDestroy]
	{
		if (InVfxComponent)
		{
			*InVfxComponent = AttachVfx(InVfxName, InAttachTo, InSocketName, InLocation, InRotation, InAttachType, bAutoDestroy);
			if (bDeactivate) InVfxComponent->Get()->Deactivate();
		}
		else AttachVfx(InVfxName, InAttachTo, InSocketName, InLocation, InRotation, InAttachType, bAutoDestroy);
	});
}

void UPrimaryDataAssetsManager::SpawnVfx(const EVfx InVfxName, const FVector& InLocation, const FRotator& InRotation, const TMap<FName, FNiagaraAttribute>& InAttributes)
{
	if (InAttributes.IsEmpty())
	{
		SpawnVfx(InVfxName, InLocation, InRotation);
		return;
	}
	const TObjectPtr<UNiagaraComponent> Component = SpawnVfx(InVfxName, InLocation, InRotation);
	ApplyAttributesToVfxComponent(InAttributes, Component);
}

void UPrimaryDataAssetsManager::SpawnOrLoadVfx(const EVfx InVfxName, const FVector& InLocation, const FRotator& InRotation, const TMap<FName, FNiagaraAttribute>& InAttributes)
{
	if (InVfxName == EVfx::None) return;
	if (GetVfxData(InVfxName)->GetVfxEffect().IsValid()) SpawnVfx(InVfxName, InLocation, InRotation, InAttributes);
	else GetVfxData(InVfxName)->LoadVfx(GetWorld(), [this, InVfxName, InLocation, InRotation, InAttributes] { SpawnVfx(InVfxName, InLocation, InRotation, InAttributes); });
}

void UPrimaryDataAssetsManager::ApplyAttributesToVfxComponent(const TMap<FName, FNiagaraAttribute>& InAttributes, const TObjectPtr<UNiagaraComponent>& Component)
{
	for (const TPair<FName, FNiagaraAttribute> Attribute : InAttributes)
	{
		float NewValue = Attribute.Value.Value;
		if (!Attribute.Value.bIsReplacingValue)
		{
			if (FMath::IsNearlyEqual(Attribute.Value.Value, 0.f, 0.001f)) continue;
			const float ComponentAttributeValue = GetVfxAttributeValue(Component, Attribute.Key);
			NewValue = ComponentAttributeValue + ComponentAttributeValue * (Attribute.Value.Value / 100.f);
		}
		Component->SetVariableFloat(Attribute.Key, NewValue);
	}
}

bool UPrimaryDataAssetsManager::IsGameRunning() { return IsValid(this) && GetWorld() && !GetWorld()->HasAnyFlags(RF_BeginDestroyed | RF_FinishDestroyed); }

EPoolObject UPrimaryDataAssetsManager::GetProjectileNameByCharacterName(ECharacterType InCharacterType)
{
	switch (InCharacterType)
	{
		case ECharacterType::Ranger: return EPoolObject::RangerProjectile;
		case ECharacterType::Minion: return EPoolObject::MinionProjectile;
		case ECharacterType::Trickster: return EPoolObject::TricksterProjectile;
		case ECharacterType::Spy: return EPoolObject::SpyProjectile;
		case ECharacterType::Tank: return EPoolObject::TankProjectile;
		default: return EPoolObject::RangerProjectile;
	}
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
	FillContainers(WeaponsMap, WeaponContainers, "Weapon");
	WeaponsMap.Empty();
}

void UPrimaryDataAssetsManager::SetupSoundsPrimaryAssets()
{
	TMap<FName, ESound> SoundMap;
	SoundMap.Add("PA_WaveStart", ESound::WaveStart);
	SoundMap.Add("PA_WaveCountdown", ESound::WaveCountdown);
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
	SoundMap.Add("PA_RangerSlowMotion", ESound::RangerSlowMotion);
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
	VfxsMap.Add("PA_StopTimeAbility", EVfx::StopTimeAbility);
	VfxsMap.Add("PA_StopTimeAbilityReverse", EVfx::StopTimeAbilityReverse);
	VfxsMap.Add("PA_MinionMuzzle", EVfx::MinionMuzzle);
	VfxsMap.Add("PA_MinionBulletTrail", EVfx::MinionBulletTrail);
	VfxsMap.Add("PA_MinionBulletImpact", EVfx::MinionBulletImpact);
	VfxsMap.Add("PA_TricksterMuzzle", EVfx::TricksterMuzzle);
	VfxsMap.Add("PA_TricksterFpsBulletTrail", EVfx::TricksterFpsBulletTrail);
	VfxsMap.Add("PA_TricksterBulletImpact", EVfx::TricksterBulletImpact);
	VfxsMap.Add("PA_TricksterBulletTrail", EVfx::TricksterBulletTrail);
	VfxsMap.Add("PA_TricksterPoisonImpact", EVfx::TricksterPoisonImpact);
	VfxsMap.Add("PA_TricksterPoison", EVfx::TricksterPoison);
	VfxsMap.Add("PA_TricksterEmpImpact", EVfx::TricksterEmpImpact);
	VfxsMap.Add("PA_SpyMuzzle", EVfx::SpyMuzzle);
	VfxsMap.Add("PA_SpyBulletTrail", EVfx::SpyBulletTrail);
	VfxsMap.Add("PA_SpyBulletImpact", EVfx::SpyBulletImpact);
	VfxsMap.Add("PA_SpyFpsBulletTrail", EVfx::SpyFpsBulletTrail);
	VfxsMap.Add("PA_TankMuzzle", EVfx::TankMuzzle);
	VfxsMap.Add("PA_TankBulletTrail", EVfx::TankBulletTrail);
	VfxsMap.Add("PA_TankBulletImpact", EVfx::TankBulletImpact);
	VfxsMap.Add("PA_TankFpsBulletTrail", EVfx::TankFpsBulletTrail);
	FillContainers(VfxsMap, VfxContainers, "Vfx");
	VfxsMap.Empty();
}

void UPrimaryDataAssetsManager::SetAndPlaySoundCallback(const TObjectPtr<UAudioComponent>& InAudioComp, const ESound InSoundName)
{
	InAudioComp->SetSound(GetSoundData(InSoundName)->Sound.Get());
	InAudioComp->Play();
}

template<typename T, typename U>
void UPrimaryDataAssetsManager::FillContainers(TMap<FName, T>& Map, TMap<T, TObjectPtr<U>>& Containers, FName Type)
{
	TArray<FAssetData> Assets;
	UAssetManager::Get().GetPrimaryAssetDataList(FPrimaryAssetType(Type), Assets);
	for (int i = 0; i < Assets.Num(); i++) Containers.Add(Map[Assets[i].AssetName], Cast<U>(Assets[i].GetAsset()));
	Assets.Empty();
}

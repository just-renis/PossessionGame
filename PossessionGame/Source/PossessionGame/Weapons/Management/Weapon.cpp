#include "Weapon.h"
#include "HandsAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Data Assets/AI/Base Data/AIWeaponData.h"
#include "PossessionGame/Data Assets/Player/Base Data/PlayerWeaponData.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_WeaponContainer.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Enums/EVfxType.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameGameModeBase.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/UpgradeManager.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("handsMesh"));
	HandsMesh->SetupAttachment(RootComponent);
	HandsMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HandsMesh->SetHiddenInGame(true);
}

void AWeapon::AI_Shoot()
{
	MuzzleFlashLogic();
	ShootTimer = 0.f;
}

void AWeapon::AI_ResetShootCycles() { ShootCyclesUntilReload = AI_Data->GetShootCyclesUntilReload(); }

bool AWeapon::P_CanReload(const int32 InOtherAmmoSource)
{
	if (!Proprietor->GetCanReload()) return false;
	return !HandsAnimInstance->GetIsReloading() && (InOtherAmmoSource != -1 ? InOtherAmmoSource > 0 : TotalAmmo > 0 && CurrentAmmo != MagazineCap);
}

bool AWeapon::P_CanShoot(const bool bIsFirstWeapon) { return ShootTimer >= ShootCooldown && CurrentAmmo > 0 && !HandsAnimInstance->GetIsReloading(); }

void AWeapon::P_SetWalkingSpeed(const float InSpeed) { if (HandsAnimInstance) HandsAnimInstance->SetSpeed(InSpeed); }

void AWeapon::P_Reload(const int32 InOtherAmmoSource)
{
	if (InOtherAmmoSource != -1) TotalAmmo = InOtherAmmoSource;
	const int32 AmmoNeeded = MagazineCap - CurrentAmmo;
	if (TotalAmmo >= AmmoNeeded)
	{
		CurrentAmmo += AmmoNeeded;
		TotalAmmo -= AmmoNeeded;
	}
	else
	{
		CurrentAmmo += TotalAmmo;
		TotalAmmo = 0;
	}
}

void AWeapon::P_Shoot(const bool bIsFirstWeapon)
{
	MuzzleFlashLogic();
	MakeShootingCameraShake();
	CurrentAmmo--;
	ShootTimer = 0.f;
	bIsHitScanEnabled ? HitScanLogic() : ShootProjectile(EPoolObject::RangerProjectile);
	P_UpdateAmmoTextHUD();
}

void AWeapon::P_InitialSetupTransfer()
{
	bIsPossessed = true;
	P_Setup();
	SetCastShadow(false);
}

void AWeapon::P_SetupTransfer(const bool bSetToFirstHand)
{
	DelegateManager->OnHandsReloadMontageEnd.BindUObject(this, &AWeapon::P_UpdateAmmoTextHUD);
	P_InitialSetupTransfer();
	if (bSetToFirstHand) P_AttachHandsToCamera(P_Data->GetFirstWeaponLocation(), P_Data->GetFirstWeaponRotation(), P_Data->GetFirstWeaponScale());
	else P_AttachHandsToCamera(P_Data->GetSecondWeaponLocation(), P_Data->GetSecondWeaponRotation(), P_Data->GetSecondWeaponScale());
}

void AWeapon::P_UpdateAmmoTextHUD() { DelegateManager->OnWeaponAmmoChangeTwoParams.Broadcast(CurrentAmmo, TotalAmmo); }

void AWeapon::P_AttachHandsToCamera(const FVector InLocation, const FRotator InRotation, const FVector InScale)
{
	HandsMesh->SetHiddenInGame(false);
	HandsMesh->AttachToComponent(GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetTransformComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	HandsMesh->SetRelativeLocationAndRotation(InLocation, InRotation);
	if (InScale != FVector::Zero()) HandsMesh->SetRelativeScale3D(InScale);
	if (Mesh) P_AttachWeaponToHands(HandsMesh);
}

void AWeapon::P_AttachWeaponToHands(const TObjectPtr<USkeletalMeshComponent>& InHandsMesh, FName InSocketName)
{
	Mesh->AttachToComponent(InHandsMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, InSocketName);
}

void AWeapon::P_SetWalkSpread(const bool bState)
{
	bWalkSpreadEnabled = bState;
	P_Data->SetAllProjectilesWalkSpread(bState);
}

void AWeapon::DestroyThis()
{
	DelegateManager->OnLevelUp.RemoveAll(this);
	Destroy();
}

void AWeapon::SetToOriginalSettings()
{
	SetWeaponMeshScaleToOriginal();
	SetCastShadow(true);
}

void AWeapon::DeactivateWeapon()
{
	bIsPossessed = false;
	Mesh->SetAnimInstanceClass(nullptr);
	if (HandsMesh) DisableHandsMesh();
	DetachWeapon();
}

void AWeapon::AttachToSocket(const TObjectPtr<USkeletalMeshComponent>& OwnerMesh) const
{
	Mesh->AttachToComponent(OwnerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("AIWeaponSocket1"));
}

void AWeapon::ShootProjectile(const EPoolObject InProjectileName, bool bOverrideRotation, bool bFromFirstWeapon)
{
	const int32 ProjectilesAmount = GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(bIsPossessed, InProjectileName, EUpgradeAttribute::ProjectilesAmount);
	GetProjectileData(InProjectileName).SetProprietor(Proprietor);
	GetProjectileData(InProjectileName).SetAmount(ProjectilesAmount + 1);
	const FName& ProjectileSocketName = GetProjectileData(InProjectileName).ProjectileSocketName;
	const FRotator BaseRotation = bOverrideRotation ? Proprietor->GetActorRotation() : GetSocketRotation(ProjectileSocketName);
	if (GetProjectileData(InProjectileName).Amount > 1)
	{
		const FAdditionalProjectiles ProjectilesData = GetProjectileData(InProjectileName).GetMultiProjectileData(GetProjectileData(InProjectileName).Amount);
		for (int32 i = 0; i < ProjectilesData.Data.Num(); i++)
		{
			GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(InProjectileName, GetSocketLocation(ProjectileSocketName), BaseRotation.RotateVector(ProjectilesData.Data[i].Direction).Rotation())
			->Init(GetProjectileData(InProjectileName), BaseRotation.RotateVector(ProjectilesData.Data[i].Direction).Rotation(), GetSocketScale(ProjectileSocketName), i);
		}
	}
	else
	{
		GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(InProjectileName, GetSocketLocation(ProjectileSocketName), BaseRotation)
		->Init(GetProjectileData(InProjectileName), BaseRotation, GetSocketScale(ProjectileSocketName));
	}
}

void AWeapon::PostProprietorSetup()
{
	AssetsManager->AttachOrLoadVfx(AssetsManager->GetProjectileVfxName(Proprietor->GetCharacterName(), EVfxType::Muzzle), &MuzzleFlashComponent, Mesh, FName("MuzzleSocket"), true);
}

void AWeapon::AI_Setup()
{
	AI_LoadDataIfNotLoaded(Name);
	bIsReloadEnabled = AI_Data->GetIsReloadEnabled();
	CurrentAmmo = AI_Data->GetCurrentAmmo();
	MagazineCap = AI_Data->GetMagazineCap();
	ShootCooldown = AI_Data->GetShootCooldown();
	ShootCyclesUntilReload = AI_Data->GetShootCyclesUntilReload();
}

void AWeapon::AI_LoadDataIfNotLoaded(const EWeapon InPrimaryDataAssetName)
{
	if (!AI_Data) AI_Data = AssetsManager->GetWeaponData(InPrimaryDataAssetName)->AI_Data.LoadSynchronous();
}

void AWeapon::P_Setup()
{
	P_LoadDataIfNotLoaded(Name);
	bIsReloadEnabled = P_Data->GetIsReloadEnabled();
	CurrentAmmo = P_Data->GetCurrentAmmo();
	MagazineCap = P_Data->GetMagazineCap();
	TotalAmmo = P_Data->GetTotalAmmo();
	HitScanDamage = P_Data->GetHitScanDamage();
	HitScanDamage += P_Data->GetHitScanDamage() * (Proprietor->GetGameInstance()->GetSaveSlot()->GetLevelDamageIncrease(false) / 100.f);
	ShootCooldown = P_Data->GetShootCooldown();
	bIsHitScanEnabled = P_Data->GetHitScanEnabled();
	P_SetupUpgradesAppliance();
	DelegateManager->ApplyUpgradeOnSelection.AddUObject(this, &AWeapon::P_ApplyUpgradeOnSelection);
	DelegateManager->OnLevelUp.AddUObject(this, &AWeapon::P_SetupLevelUpAppliance);
}

void AWeapon::P_LoadDataIfNotLoaded(const EWeapon InPrimaryDataAssetName)
{
	if (!P_Data) P_Data = AssetsManager->GetWeaponData(InPrimaryDataAssetName)->P_Data.LoadSynchronous();
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	AssetsManager = GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>();
	DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	if (Name != EWeapon::MinionWeapon) SetMeshes(Name);
	AI_Setup();
}

void AWeapon::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	ShootTimer += DeltaTime;
}

void AWeapon::HitScanLogic()
{
	FHitResult HitResult;
	const FVector StartLocation = Proprietor->GetCamera()->GetComponentLocation();
	FVector EndLocation = StartLocation + Proprietor->GetCamera()->GetForwardVector() * 15000.f;
	EndLocation += FVector(FMath::RandRange(-P_GetHitScanCalcSpread(), P_GetHitScanCalcSpread()), FMath::RandRange(-P_GetHitScanCalcSpread(), P_GetHitScanCalcSpread()), FMath::RandRange(-P_GetHitScanCalcSpread(), P_GetHitScanCalcSpread()));
	AssetsManager->SpawnOrLoadVfx(AssetsManager->GetProjectileVfxName(Proprietor->GetCharacterName(), EVfxType::FpsTrail), GetSocketLocation("ProjectileSocket"), GetSocketRotation("ProjectileSocket"));
	if (Proprietor->GetGameModeBase()->GetIsDebugModeEnabled()) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 2.f);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility))
	{
		AssetsManager->SpawnOrLoadVfx(AssetsManager->GetProjectileVfxName(Proprietor->GetCharacterName(), EVfxType::Impact), HitResult.Location, HitResult.ImpactNormal.Rotation());
		if (HitResult.GetActor()->ActorHasTag("Enemy") || HitResult.GetActor()->ActorHasTag("Destructible")) DealPointDamage(HitResult);
	}
}

void AWeapon::MakeShootingCameraShake() const
{
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(ShootingCameraShake);
}

void AWeapon::SetMeshes(const EWeapon InPrimaryDataAssetName)
{
	if (!Mesh->GetSkeletalMeshAsset()) Mesh->SetSkeletalMesh(AssetsManager->GetWeaponData(InPrimaryDataAssetName)->WeaponMesh.Get());
	if (!HandsMesh->GetSkeletalMeshAsset()) HandsMesh->SetSkeletalMesh(AssetsManager->GetWeaponData(InPrimaryDataAssetName)->HandsMesh.Get());
	OriginalScale = Mesh->GetRelativeScale3D();
}

void AWeapon::SetupHandsAnimInstance()
{
	HandsAnimInstance = Cast<UHandsAnimInstance>(HandsMesh->GetAnimInstance());
	HandsAnimInstance->SetWeaponName(Name);
	HandsAnimInstance->SetProprietor(Proprietor);
}

void AWeapon::DetachWeapon()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Mesh->SetSimulatePhysics(true);
}

void AWeapon::PlaySound(const ESound InSoundName) { Sounds[InSoundName]->Sound ? Sounds[InSoundName]->Play() : AssetsManager->ConfigureSound(Sounds[InSoundName], InSoundName); }

void AWeapon::MuzzleFlashLogic() const
{
	if (MuzzleFlashComponent)
	{
		if (MuzzleFlashComponent->IsActive()) MuzzleFlashComponent->Deactivate();
		MuzzleFlashComponent->Activate();
	}
}

void AWeapon::DisableHandsMesh() const
{
	HandsMesh->SetHiddenInGame(true);
	HandsMesh->SetAnimInstanceClass(nullptr);
}

void AWeapon::DealPointDamage(const FHitResult& InHitResult)
{
	const bool bIsCritical = InHitResult.BoneName == "head";
	(void) DelegateManager->OnPlayerDamageDealt.ExecuteIfBound(bIsCritical);
	AssetsManager->PlaySound2D(bIsCritical ? ESound::CriticalDamageTick : ESound::DamageTick);
	UGameplayStatics::ApplyPointDamage(InHitResult.GetActor(), bIsCritical ? HitScanDamage * 2 : HitScanDamage, (InHitResult.Location - GetActorLocation()).GetSafeNormal(), InHitResult, nullptr, this, UDamageType::StaticClass());
}

void AWeapon::P_SetupUpgradesAppliance()
{
	HitScanDamage += GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(true, Proprietor->GetCharacterName(), EUpgradeAttribute::FlatDamage);
	const float DamagePercentage = GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(true, Proprietor->GetCharacterName(), EUpgradeAttribute::DamagePercentage);
	if (DamagePercentage > 0.f) HitScanDamage += P_Data->GetHitScanDamage() * (DamagePercentage / 100.f);
}

void AWeapon::P_SetupLevelUpAppliance()
{
	HitScanDamage -= P_Data->GetHitScanDamage() * (Proprietor->GetGameInstance()->GetSaveSlot()->GetLevelDamageIncrease(true) / 100.f);
	HitScanDamage += P_Data->GetHitScanDamage() * (Proprietor->GetGameInstance()->GetSaveSlot()->GetLevelDamageIncrease(false) / 100.f);
}

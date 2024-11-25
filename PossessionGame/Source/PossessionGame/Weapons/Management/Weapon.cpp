#include "Weapon.h"
#include "HandsAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Data Assets/AI/Base Data/AIWeaponData.h"
#include "PossessionGame/Data Assets/Player/Base Data/PlayerWeaponData.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_VfxContainer.h"
#include "PossessionGame/Enums/EVfxType.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

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
	ShootProjectile(EPoolObject::RangerProjectile);
}

void AWeapon::AI_ResetShootCycles() { ShootCyclesUntilReload = AI_Data->GetShootCyclesUntilReload(); }

bool AWeapon::P_CanReload(int32 OtherAmmoSource)
{
	return !HandsAnimInstance->GetIsReloading() && (OtherAmmoSource != -1 ? OtherAmmoSource > 0 : TotalAmmo > 0 && CurrentAmmo != MagazineCap);
}

bool AWeapon::P_CanShoot(bool bIsFirstWeapon) { return ShootTimer >= ShootCooldown && CurrentAmmo > 0 && !HandsAnimInstance->GetIsReloading(); }

void AWeapon::P_SetWalkingSpeed(float Speed) { if (HandsAnimInstance) HandsAnimInstance->SetSpeed(Speed); }

void AWeapon::P_Reload(int32 OtherAmmoSource)
{
	if (OtherAmmoSource != -1) TotalAmmo = OtherAmmoSource;
	int32 AmmoNeeded = MagazineCap - CurrentAmmo;
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

void AWeapon::P_Shoot(bool bIsFirstWeapon)
{
	MuzzleFlashLogic();
	MakeCameraShake();
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

void AWeapon::P_SetupTransfer(bool bSetToFirstHand)
{
	DelegateManager->OnHandsReloadMontageEnd.BindUObject(this, &AWeapon::P_UpdateAmmoTextHUD);
	P_InitialSetupTransfer();
	if (bSetToFirstHand) P_AttachHandsToCamera(P_Data->GetFirstWeaponLocation(), P_Data->GetFirstWeaponRotation(), P_Data->GetFirstWeaponScale());
	else P_AttachHandsToCamera(P_Data->GetSecondWeaponLocation(), P_Data->GetSecondWeaponRotation(), P_Data->GetSecondWeaponScale());
}

void AWeapon::P_UpdateAmmoTextHUD() { DelegateManager->OnWeaponAmmoChangeTwoParams.Broadcast(CurrentAmmo, TotalAmmo); }

void AWeapon::P_AttachHandsToCamera(FVector Location, FRotator Rotation, FVector Scale)
{
	HandsMesh->SetHiddenInGame(false);
	HandsMesh->AttachToComponent(GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetTransformComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	HandsMesh->SetRelativeLocationAndRotation(Location, Rotation);
	if (Scale != FVector::Zero()) HandsMesh->SetRelativeScale3D(Scale);
	if (Mesh) P_AttachWeaponToHands(HandsMesh);
}

void AWeapon::P_AttachWeaponToHands(TObjectPtr<USkeletalMeshComponent> InHandsMesh, FName SocketName)
{
	Mesh->AttachToComponent(InHandsMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
}

void AWeapon::P_SetWalkSpread(bool bState)
{
	bWalkSpreadEnabled = bState;
	P_Data->SetAllProjectilesWalkSpread(bState);
}

void AWeapon::DestroyThis()
{
	UKismetSystemLibrary::K2_ClearTimer(this, "LaserSightLogic");
	Destroy();
}

void AWeapon::SetToOriginalSettings()
{
	SetWeaponMeshScaleToOriginal();
	SetCastShadow(true);
}

void AWeapon::DeactivateWeapon()
{
	DeactivateLaserSight();
	bIsPossessed = false;
	Mesh->SetAnimInstanceClass(nullptr);
	if (HandsMesh) DisableHandsMesh();
	DetachWeapon();
}

void AWeapon::AttachToSocket(TObjectPtr<USkeletalMeshComponent> OwnerMesh)
{
	Mesh->AttachToComponent(OwnerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("AIWeaponSocket1"));
}

void AWeapon::ShootProjectile(EPoolObject ProjectileName)
{
	GetProjectileData(ProjectileName).SetProprietor(Proprietor);
	GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(GetProjectileData(ProjectileName), GetSocketLocation(GetProjectileData(ProjectileName).ProjectileSocketName), GetSocketRotation(GetProjectileData(ProjectileName).ProjectileSocketName), GetSocketScale(GetProjectileData(ProjectileName).ProjectileSocketName));
}

void AWeapon::PostProprietorSetup() { AttachAndLoadVfxs(); }

void AWeapon::AI_Setup()
{
	AI_LoadDataIfNotLoaded(Name);
	bIsReloadEnabled = AI_Data->GetIsReloadEnabled();
	CurrentAmmo = AI_Data->GetCurrentAmmo();
	MagazineCap = AI_Data->GetMagazineCap();
	ShootCooldown = AI_Data->GetShootCooldown();
	ShootCyclesUntilReload = AI_Data->GetShootCyclesUntilReload();
}

void AWeapon::AI_LoadDataIfNotLoaded(EWeapon PrimaryDataAssetName)
{
	if (!AI_Data) AI_Data = AssetsManager->GetWeaponData(PrimaryDataAssetName)->AI_Data.LoadSynchronous();
}

void AWeapon::P_Setup()
{
	P_LoadDataIfNotLoaded(Name);
	bIsReloadEnabled = P_Data->GetIsReloadEnabled();
	CurrentAmmo = P_Data->GetCurrentAmmo();
	MagazineCap = P_Data->GetMagazineCap();
	TotalAmmo = P_Data->GetTotalAmmo();
	HitScanDamage = P_Data->GetHitScanDamage();
	ShootCooldown = P_Data->GetShootCooldown();
	bIsHitScanEnabled = P_Data->GetHitScanEnabled();
	if (P_Data->GetLaserSightEnabled()) UKismetSystemLibrary::K2_SetTimer(this, "LaserSightLogic", 0.01f, true);
}

void AWeapon::P_LoadDataIfNotLoaded(EWeapon PrimaryDataAssetName)
{
	if (!P_Data) P_Data = AssetsManager->GetWeaponData(PrimaryDataAssetName)->P_Data.LoadSynchronous();
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	AssetsManager = GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>();
	DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	LoadSounds();
	if (Name != EWeapon::MinionWeapon) SetMeshes(Name);
	AI_Setup();
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ShootTimer += DeltaTime;
}

void AWeapon::HitScanLogic()
{
	FHitResult HitResult;
	FVector StartLocation = Proprietor->GetCamera()->GetComponentLocation();
	FVector EndLocation = StartLocation + Proprietor->GetCamera()->GetForwardVector() * 15000.f;
	EndLocation += FVector(FMath::RandRange(-P_GetHitScanCalcSpread(), P_GetHitScanCalcSpread()), FMath::RandRange(-P_GetHitScanCalcSpread(), P_GetHitScanCalcSpread()), FMath::RandRange(-P_GetHitScanCalcSpread(), P_GetHitScanCalcSpread()));
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, AssetsManager->GetVfxData(AssetsManager->GetVfxName(Proprietor->GetCharacterName(), EVfxType::FpsTrail))->GetVfxEffect().Get(), StartLocation, HitResult.ImpactNormal.Rotation());
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 2.f);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility))
	{
		SpawnOrLoadHitScanImpactVfx(AssetsManager->GetVfxName(Proprietor->GetCharacterName(), EVfxType::Impact), HitResult.Location, HitResult.ImpactNormal.Rotation());
		if (HitResult.GetActor()->ActorHasTag("Enemy") || HitResult.GetActor()->ActorHasTag("Destructible"))
		{
			UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), HitResult.BoneName == "Head" ? HitScanDamage * 2 : HitScanDamage, (HitResult.Location - GetActorLocation()).GetSafeNormal(), HitResult, nullptr, this, UDamageType::StaticClass());
		}
	}
}

void AWeapon::MakeCameraShake()
{
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(ShootingCameraShake);
}

void AWeapon::SetMeshes(EWeapon PrimaryDataAssetName)
{
	if (!Mesh->GetSkeletalMeshAsset()) Mesh->SetSkeletalMesh(AssetsManager->GetWeaponData(PrimaryDataAssetName)->WeaponMesh.Get());
	if (!HandsMesh->GetSkeletalMeshAsset()) HandsMesh->SetSkeletalMesh(AssetsManager->GetWeaponData(PrimaryDataAssetName)->HandsMesh.Get());
	OriginalScale = Mesh->GetRelativeScale3D();
}

void AWeapon::SetupHandsAnimInstance()
{
	HandsAnimInstance = Cast<UHandsAnimInstance>(HandsMesh->GetAnimInstance());
	HandsAnimInstance->SetWeaponName(Name);
}

void AWeapon::DetachWeapon()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Mesh->SetSimulatePhysics(true);
}

void AWeapon::PlaySound(ESound SoundName) { Sounds[SoundName]->Sound ? Sounds[SoundName]->Play() : AssetsManager->LoadAndSetSound(Sounds[SoundName], SoundName); }

void AWeapon::MuzzleFlashLogic()
{
	if (MuzzleFlashComponent)
	{
		if (MuzzleFlashComponent->IsActive()) MuzzleFlashComponent->Deactivate();
		MuzzleFlashComponent->Activate();
	}
}

void AWeapon::DisableHandsMesh()
{
	HandsMesh->SetHiddenInGame(true);
	HandsMesh->SetAnimInstanceClass(nullptr);
}

void AWeapon::AttachMuzzleFlash(EVfx VfxName)
{
	MuzzleFlashComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(AssetsManager->GetVfxData(VfxName)->GetVfxEffect().Get(), Mesh, FName("MuzzleSocket"),
	FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
	MuzzleFlashComponent->Deactivate();
}

void AWeapon::AttachAndLoadVfxs()
{
	EVfx VfxName = AssetsManager->GetVfxName(Proprietor->GetCharacterName(), EVfxType::Muzzle);
	if (AssetsManager->GetVfxData(VfxName)->GetVfxEffect().IsValid()) AttachMuzzleFlash(VfxName);
	else AssetsManager->GetVfxData(VfxName)->LoadVfx(GetWorld(), [this, VfxName] { AttachMuzzleFlash(VfxName); });
}

void AWeapon::DeactivateLaserSight()
{
	if (LaserSight)
	{
		LaserSight->Deactivate();
		LaserSight = nullptr;
	}
}

void AWeapon::SpawnOrLoadHitScanImpactVfx(EVfx VfxName, const FVector& InLocation, const FRotator& InRotation)
{
	if (AssetsManager->GetVfxData(VfxName)->GetVfxEffect().IsValid()) SpawnHitScanImpactVfx(VfxName, InLocation, InRotation);
	else AssetsManager->GetVfxData(VfxName)->LoadVfx(GetWorld(), [this, VfxName, InLocation, InRotation] { SpawnHitScanImpactVfx(VfxName, InLocation, InRotation); });
}

void AWeapon::SpawnHitScanImpactVfx(EVfx VfxName, const FVector& InLocation, const FRotator& InRotation)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, AssetsManager->GetVfxData(VfxName)->GetVfxEffect().Get(), InLocation, InRotation);
}

void AWeapon::LaserSightLogic()
{
	if (!LaserSight) LaserSight = UNiagaraFunctionLibrary::SpawnSystemAttached(LaserSightSystem.LoadSynchronous(), Mesh, FName("LaserSightSocket"), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
	else
	{
		FVector StartLocation = GetSocketLocation(FName("LaserSightSocket"));
		FVector TraceEnd = StartLocation + GetSocketRotation(FName("LaserSightSocket")).Vector() * 500;
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, TraceEnd, ECC_Visibility, QueryParams)) LaserSight->SetNiagaraVariableVec3(FName("User.Beam End").ToString(), HitResult.Location);
		else LaserSight->SetNiagaraVariableVec3(FName("User.Beam End").ToString(), TraceEnd);
	}
}

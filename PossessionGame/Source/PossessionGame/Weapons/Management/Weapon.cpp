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
#include "PossessionGame/Utilities/Data Utilities/RunSaveSlot.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
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
	if (TotalAmmo == 0 && CurrentAmmo == 0) (void)DelegateManager->OnOutOfAmmo.ExecuteIfBound();
	return !HandsAnimInstance->GetIsReloading() && (InOtherAmmoSource != -1 ? InOtherAmmoSource > 0 : TotalAmmo > 0 && CurrentAmmo != MagazineCap);
}

bool AWeapon::P_CanShoot(const bool bIsFirstWeapon) { return ShootTimer >= ShootCooldown && CurrentAmmo > 0 && !HandsAnimInstance->GetIsReloading(); }

void AWeapon::P_SetWalkingSpeed(const float InSpeed) const { if (HandsAnimInstance) HandsAnimInstance->SetSpeed(InSpeed); }

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
	P_SetVariationMaterial();
	P_ChangeVfxVariation();
}

void AWeapon::P_UpdateAmmoTextHUD() { DelegateManager->OnWeaponAmmoChangeTwoParams.Broadcast(CurrentAmmo, TotalAmmo); }

void AWeapon::P_AttachHandsToCamera(const FVector& InLocation, const FRotator& InRotation, const FVector& InScale)
{
	HandsMesh->SetHiddenInGame(false);
	HandsMesh->AttachToComponent(GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetTransformComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	HandsMesh->SetRelativeLocationAndRotation(InLocation, InRotation);
	if (InScale != FVector::ZeroVector) HandsMesh->SetRelativeScale3D(InScale);
	if (Mesh)
	{
		Mesh->SetRelativeLocationAndRotation(InLocation, InRotation);
		Mesh->SetRelativeScale3D(InScale);
		P_AttachWeaponToHands(HandsMesh);
	}
}

void AWeapon::P_AttachWeaponToHands(const TObjectPtr<USkeletalMeshComponent>& InHandsMesh, const FName& InSocketName)
{
	Mesh->AttachToComponent(InHandsMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, InSocketName);
}

void AWeapon::P_SetWalkSpread(const bool bState)
{
	bWalkSpreadEnabled = bState;
	P_Data->SetAllProjectilesWalkSpread(bState);
}

void AWeapon::P_SetVariationMaterial()
{
	const TObjectPtr<UMaterialInstanceDynamic> MI_Variation = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this);
	if (!MI_Variation) return;
	GetMesh()->SetMaterial(0, MI_Variation);
	MI_Variation->SetScalarParameterValue("Hue", P_Data->GetVariationHueValue(Proprietor->GetVariation()));
	MI_Variation->SetScalarParameterValue("EmissiveHue", P_Data->GetVariationHueValue(Proprietor->GetVariation()));
}

void AWeapon::P_ChangeVfxVariation()
{
	if (MuzzleFlashComponent)
	{
		AssetsManager->ChangeVfxVariation(AssetsManager->GetVfxData(AssetsManager->GetProjectileVfxName(Proprietor->GetCharacterName(), EVfxType::Muzzle)), nullptr, MuzzleFlashComponent, false, Proprietor->GetVariation());
	}
}

void AWeapon::P_ReduceShootCooldown(const float InPercentage) { ShootCooldown -= P_Data->GetShootCooldown() * InPercentage * 0.01f; }

void AWeapon::P_ResetShootCooldown() { ShootCooldown = P_Data->GetShootCooldown(); }

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

void AWeapon::SetProprietor(const TObjectPtr<AEnemy> InProprietor)
{
	Proprietor = InProprietor;
}

void AWeapon::AttachToSocket(const TObjectPtr<USkeletalMeshComponent>& OwnerMesh) const
{
	Mesh->AttachToComponent(OwnerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("AIWeaponSocket1"));
}

void AWeapon::ShootProjectile(const EPoolObject InProjectileName, bool bFromFirstWeapon)
{
	FProjectileData* ProjectileData = GetProjectileData(InProjectileName);
	if (!ProjectileData) return;
	const int32 ProjectilesAmount = GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(bIsPossessed, InProjectileName, EUpgradeAttribute::ProjectilesAmount);
	ProjectileData->SetProprietor(Proprietor);
	ProjectileData->SetAmount(ProjectilesAmount + 1);
	const FName& ProjectileSocketName = ProjectileData->ProjectileSocketName;
	const FRotator BaseRotation = GetSocketRotation(ProjectileSocketName);
	if (ProjectileData->Amount > 1)
	{
		const FAdditionalProjectiles AdditionalProjectilesData = ProjectileData->GetMultiProjectileData(ProjectileData->Amount);
		for (int32 i = 0; i < AdditionalProjectilesData.Data.Num(); i++)
		{
			GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(InProjectileName, GetSocketLocation(ProjectileSocketName), BaseRotation.RotateVector(AdditionalProjectilesData.Data[i].Direction).Rotation())
			->Init(*ProjectileData, BaseRotation.RotateVector(AdditionalProjectilesData.Data[i].Direction).Rotation(), GetSocketScale(ProjectileSocketName), i);
		}
	}
	else
	{
		GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(InProjectileName, GetSocketLocation(ProjectileSocketName), BaseRotation)
		->Init(*ProjectileData, BaseRotation, GetSocketScale(ProjectileSocketName));
	}
}

void AWeapon::PostProprietorSetup() { SetupHandsAnimInstance(); }

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
	HitScanDamage = DefaultHitScanDamage = P_Data->GetHitScanDamage();
	P_VariationHitScanDamageAppliance();
	HitScanDamage += DefaultHitScanDamage * Proprietor->GetGameInstance()->GetSaveSlot()->GetLevelDamageIncrease() * 0.01f;
	ShootCooldown = P_Data->GetShootCooldown();
	bIsHitScanEnabled = P_Data->GetHitScanEnabled();
	DelegateManager->OnLevelUp.AddUObject(this, &AWeapon::P_SetupLevelUpAppliance);
}

void AWeapon::P_LoadDataIfNotLoaded(const EWeapon InPrimaryDataAssetName)
{
	if (!P_Data) P_Data = AssetsManager->GetWeaponData(InPrimaryDataAssetName)->P_Data.LoadSynchronous();
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	AssetsManager = GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>();
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
	AssetsManager->SpawnVfx(AssetsManager->GetProjectileVfxName(Proprietor->GetCharacterName(), EVfxType::FpsTrail), nullptr, GetSocketLocation("FpsProjectileSocket"), GetSocketRotation("FpsProjectileSocket"), Proprietor->GetVariation());
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility))
	{
		AssetsManager->SpawnVfx(AssetsManager->GetProjectileVfxName(Proprietor->GetCharacterName(), EVfxType::Impact), nullptr, HitResult.Location, HitResult.ImpactNormal.Rotation(), Proprietor->GetVariation());
		if (HitResult.GetActor()->ActorHasTag("Enemy") || HitResult.GetActor()->ActorHasTag("Destructible")) DealPointDamage(HitResult);
	}
}

void AWeapon::MakeShootingCameraShake() const { UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(ShootingCameraShake); }

void AWeapon::SetMeshes(const EWeapon InPrimaryDataAssetName)
{
	if (!Mesh->GetSkeletalMeshAsset()) Mesh->SetSkeletalMesh(AssetsManager->GetWeaponData(InPrimaryDataAssetName)->WeaponMesh.Get());
	if (!HandsMesh->GetSkeletalMeshAsset()) HandsMesh->SetSkeletalMesh(AssetsManager->GetWeaponData(InPrimaryDataAssetName)->HandsMesh.Get());
	OriginalScale = Mesh->GetRelativeScale3D();
}

void AWeapon::SetupHandsAnimInstance()
{
	HandsAnimInstance = Cast<UHandsAnimInstance>(HandsMesh->GetAnimInstance());
	if (!HandsAnimInstance) return;
	HandsAnimInstance->SetWeaponName(Name);
	HandsAnimInstance->SetProprietor(Proprietor);
}

void AWeapon::DetachWeapon()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Mesh->SetSimulatePhysics(true);
}

void AWeapon::PlaySound(const ESound InSoundName, const bool bIsCallbackRequired)
{
	Sounds[InSoundName]->Sound ? Sounds[InSoundName]->Play() :
	AssetsManager->ConfigureSound(Sounds[InSoundName], bIsCallbackRequired ? [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed(); } : TFunction<bool()>(), InSoundName, bIsPossessed);
}

void AWeapon::MuzzleFlashLogic()
{
	if (!MuzzleFlashComponent)
	{
		AssetsManager->AttachVfx(AssetsManager->GetProjectileVfxName(Proprietor->GetCharacterName(), EVfxType::Muzzle), [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed(); },
		&MuzzleFlashComponent, Mesh, FName("MuzzleSocket"), true,FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false, Proprietor->GetVariation());
	}
	else
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
	const float SecretBoostDamage = P_ReceiveHitScanDamage() + P_ReceiveHitScanDamage() * Proprietor->GetGameInstance()->GetRunSaveSlot()->GetSecretBoost(EVariation::SecretCrateRewardDamage) * 0.01f;
	const float Damage = SecretBoostDamage * P_Data->GetDamageMultiplierFromRange(InHitResult.Distance * 0.01f);
	if (FMath::IsNearlyEqual(Damage, 0.f, 0.01f)) return;
	(void)DelegateManager->OnPlayerDamageDealt.ExecuteIfBound(bIsCritical);
	AssetsManager->PlaySound2D(bIsCritical ? ESound::CriticalDamageTick : ESound::DamageTick);
	UGameplayStatics::ApplyPointDamage(InHitResult.GetActor(), bIsCritical ? Damage * P_Data->GetCriticalDamage() : Damage,
	(InHitResult.Location - GetActorLocation()).GetSafeNormal(), InHitResult, nullptr, this, UDamageType::StaticClass());
}

void AWeapon::P_SetupLevelUpAppliance(const int32 InLevel)
{
	HitScanDamage -= DefaultHitScanDamage * Proprietor->GetGameInstance()->GetSaveSlot()->GetLevelDamageIncrease(InLevel - 1) * 0.01f;
	HitScanDamage += DefaultHitScanDamage * Proprietor->GetGameInstance()->GetSaveSlot()->GetLevelDamageIncrease(InLevel) * 0.01f;
}

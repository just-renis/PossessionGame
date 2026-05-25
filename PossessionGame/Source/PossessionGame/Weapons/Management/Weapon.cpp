#include "Weapon.h"
#include "HandsAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "NiagaraComponent.h"
#include "WeaponAnimInstance.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Data Assets/AI/Base Data/AIWeaponData.h"
#include "PossessionGame/Data Assets/Player/Base Data/PlayerWeaponData.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_WeaponContainer.h"
#include "PossessionGame/Enums/EPlayerLevelUpgradeId.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Enums/EVfxType.h"
#include "PossessionGame/Projectiles/Throwables/BreacherBarrel.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PlayerDataManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("handsMesh"));
	HandsMesh->SetupAttachment(RootComponent);
	HandsMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HandsMesh->SetHiddenInGame(true);
}

bool AWeapon::AI_CanShoot() const { return ShootTimer >= ShootCooldown * ReverseActionSpeed; }

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

bool AWeapon::P_CanShoot() { return ShootTimer >= GetShootCooldown() && CurrentAmmo > 0 && !HandsAnimInstance->GetIsReloading(); }

void AWeapon::P_SetWalkingSpeed(const float InSpeed) const { if (HandsAnimInstance) HandsAnimInstance->SetSpeed(InSpeed); }

void AWeapon::P_Shoot(const EPoolObject InProjectileName)
{
	MuzzleFlashLogic();
	MakeShootingCameraShake();
	CurrentAmmo--;
	ShootTimer = 0.f;
	bIsHitScanEnabled ? HitScanLogic() : ShootProjectile(InProjectileName, false);
	P_UpdateAmmoTextHUD();
}

void AWeapon::P_InitialSetupTransfer()
{
	if (AnimInstance) AnimInstance->Montage_Stop(0.f);
	bIsPossessed = true;
	P_Setup();
	SetCastShadow(false);
}

void AWeapon::P_SetupTransfer(const bool bSetToFirstHand)
{
	P_InitialSetupTransfer();
	if (bSetToFirstHand) P_AttachHandsToCamera(P_Data->GetFirstWeaponLocation(), P_Data->GetFirstWeaponRotation(), P_Data->GetFirstWeaponScale());
	else P_AttachHandsToCamera(P_Data->GetSecondWeaponLocation(), P_Data->GetSecondWeaponRotation(), P_Data->GetSecondWeaponScale());
	P_SetVariationMaterial();
	P_ChangeVfxVariation();
}

void AWeapon::P_UpdateAmmoTextHUD() { DelegateManager->OnWeaponAmmoChangeTwoParams.Broadcast(CurrentAmmo, TotalAmmo); }

void AWeapon::P_AttachHandsToCamera(const FVector& InLocation, const FRotator& InRotation, const FVector& InScale)
{
	HandsMesh->SetComponentTickEnabled(true);
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
	P_Data->SetupVariationMaterial(MI_Variation, Proprietor->GetVariation());
}

void AWeapon::P_ChangeVfxVariation()
{
	if (MuzzleFlashComponent)
	{
		MuzzleFlashComponent->DeactivateImmediate();
		MuzzleFlashComponent = nullptr;
	}
}

void AWeapon::P_StopReloadAnimation()
{
	if (AnimInstance) AnimInstance->StopMontage(EWeaponAnimState::Reload);
	GetWorldTimerManager().SetTimer(DisableComponentTickHandle, this, &AWeapon::DisableComponentTick, 0.1f);
}

void AWeapon::DestroyThis() { Destroy(); }

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
	if (bIsWeaponDetachable) DetachWeapon();
}

void AWeapon::SetProprietor(const TObjectPtr<AEnemy> InProprietor) { Proprietor = InProprietor; }

void AWeapon::AttachToSocket(const TObjectPtr<USkeletalMeshComponent>& OwnerMesh) const
{
	Mesh->AttachToComponent(OwnerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("AIWeaponSocket1"));
}

void AWeapon::ShootProjectile(const EPoolObject InProjectileName, const bool bIsFromAbility, const FName InProjectileSocketName, bool bFromFirstWeapon)
{
	FProjectileData* ProjectileData = GetProjectileData(InProjectileName);
	if (!ProjectileData) return;
	if (bIsFromAbility)
	{
		MuzzleLogicFromAbility();
		const int32 ProjectilesAmount = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(InProjectileName, EUpgradeAttribute::ProjectilesAmount, bIsPossessed, true);
		ProjectileData->SetAmount(ProjectilesAmount + 1);
	}
	ProjectileData->SetProprietor(Proprietor);
	const FName& ProjectileSocketName = InProjectileSocketName == NAME_None ? ProjectileData->ProjectileSocketName : InProjectileSocketName;
	const FVector BaseLocation = GetSocketLocation(ProjectileSocketName);
	const FRotator BaseRotation = GetSocketRotation(ProjectileSocketName);
	//const FVector BaseScale = Mesh->GetSocketTransform(ProjectileSocketName, RTS_Component).GetScale3D(); If ill need scale for some reason from socket for projectiles.
	if (ProjectileData->Amount > 1)
	{
		const FAdditionalProjectiles AdditionalProjectilesData = ProjectileData->GetMultiProjectileData(ProjectileData->Amount);
		for (int32 i = 0; i < AdditionalProjectilesData.Data.Num(); i++)
		{
			GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(InProjectileName, BaseLocation, BaseRotation.RotateVector(AdditionalProjectilesData.Data[i].Direction).Rotation())
			->Init(*ProjectileData, BaseRotation.RotateVector(AdditionalProjectilesData.Data[i].Direction).Rotation(), i);
		}
	}
	else GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(InProjectileName, BaseLocation, BaseRotation)->Init(*ProjectileData, BaseRotation);
}

void AWeapon::PostProprietorSetup() { SetupHandsAnimInstance(); }

void AWeapon::SetCustomDilation(const float InDilation)
{
	CustomTimeDilation = InDilation;
	HandsMesh->SetComponentTickInterval(1.f / 60.f / InDilation);
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

float AWeapon::P_GetCriticalDamage()
{
	return P_Data->GetCriticalDamage() + P_Data->GetCriticalDamage() * GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetPlayerLevelAllCriticalMultiBonus();
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
	ShootCooldown = P_Data->GetShootCooldown();
	bIsHitScanEnabled = P_Data->GetHitScanEnabled();
	DelegateManager->OnPlayerReloadMontageEnd.BindUObject(this, &AWeapon::P_ReloadAmmo);
}

void AWeapon::P_LoadDataIfNotLoaded(const EWeapon InPrimaryDataAssetName)
{
	if (!P_Data) P_Data = AssetsManager->GetWeaponData(InPrimaryDataAssetName)->P_Data.LoadSynchronous();
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	SetupHandsOptimization();
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
	FpsTrailVfxLogic();
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.bReturnPhysicalMaterial = false;
	//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 3.f);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams))
	{
		AssetsManager->SpawnVfx(AssetsManager->GetProjectileVfxName(Proprietor->GetCharacterName(), EVfxType::Impact), nullptr, HitResult.Location, HitResult.ImpactNormal.Rotation(), Proprietor->GetVariation());
		if (HitResult.GetActor()->ActorHasTag("Enemy") || HitResult.GetActor()->ActorHasTag("Destructible"))
		{
			HitResult.TraceStart = StartLocation;
			DealPointDamage(HitResult);
		}
		else if (HitResult.GetActor()->ActorHasTag("BreacherBarrel")) Cast<ABreacherBarrel>(HitResult.GetActor())->Explode();
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
		&MuzzleFlashComponent, Mesh, FName("MuzzleSocket"), Proprietor->GetVariation(), true,FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
	}
	else AssetsManager->ActivateWithVariation(AssetsManager->GetProjectileVfxName(Proprietor->GetCharacterName(), EVfxType::Muzzle), nullptr, MuzzleFlashComponent, false, Proprietor->GetVariation());
}

void AWeapon::DisableHandsMesh() const
{
	HandsMesh->SetWorldScale3D(FVector(1.f));
	HandsMesh->SetHiddenInGame(true);
	HandsMesh->SetAnimInstanceClass(nullptr);
}

void AWeapon::DealPointDamage(const FHitResult& InHitResult)
{
	const TObjectPtr<USaveSlot> SaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot();
	const bool bIsUpgradeSelected = SaveSlot->IsPlayerLevelUpgradeSelected(EPlayerLevelUpgradeId::FifthOptionB);
	const bool bIsUpgradeSelectedEightOptionA = SaveSlot->IsPlayerLevelUpgradeSelected(EPlayerLevelUpgradeId::EighthOptionA);
	
	const float AfterUpgradeApplianceDamage = DefaultHitScanDamage + P_UpgradeDamageAppliance();
	
	float AfterPlayerLevelAllGlobalDamage = AfterUpgradeApplianceDamage + P_ReceiveHitScanDamage() * GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetPlayerLevelAllGlobalDamageBonus();
	
	AfterPlayerLevelAllGlobalDamage += DefaultHitScanDamage * (bIsUpgradeSelected ? SaveSlot->GetPlayerLevel() * 0.05f : 0.f);
	
	AfterPlayerLevelAllGlobalDamage += DefaultHitScanDamage * GetGameInstance()->GetSubsystem<UPlayerDataManager>()->GetPoisonAreaStackBoost(); // Trickster Purple Variation Damage Boost.
	
	if (bIsUpgradeSelectedEightOptionA && FMath::RandRange(1, 100) < 26) AfterPlayerLevelAllGlobalDamage *= 2.f;
	
	const float RangeMultiplier = P_Data->GetDamageMultiplierFromRange(InHitResult.Distance * 0.01f);
	if (FMath::IsNearlyEqual(RangeMultiplier, 0.f, 0.01f)) return;
	if (InHitResult.GetActor()->ActorHasTag("Destructible"))
	{
		UGameplayStatics::ApplyPointDamage(InHitResult.GetActor(), AfterPlayerLevelAllGlobalDamage * RangeMultiplier, (InHitResult.Location - GetActorLocation()).GetSafeNormal(), InHitResult, nullptr, this, UDamageType::StaticClass());
		return;
	}
	
	const bool bIsCritical = InHitResult.BoneName.IsEqual(FName("head"));
	float CriticalDamageMultiplier = 1.f;
	if (bIsCritical)
	{
		CriticalDamageMultiplier = P_GetCriticalDamage();
		CriticalHitLogic();
	}
	CriticalDamageMultiplier += GetGameInstance()->GetSubsystem<UPlayerDataManager>()->GetRangerBlueVariationStackBoost(); // Ranger Blue Variation Critical Damage Boost.
	
	float FinalDamageOutput = AfterPlayerLevelAllGlobalDamage * CriticalDamageMultiplier * RangeMultiplier;
	P_ProcessFinalDamageOutput(FinalDamageOutput);
	UGameplayStatics::ApplyPointDamage(InHitResult.GetActor(), FinalDamageOutput, (InHitResult.Location - GetActorLocation()).GetSafeNormal(), InHitResult, nullptr, this, UDamageType::StaticClass());
}

void AWeapon::FpsTrailVfxLogic()
{
	AssetsManager->SpawnVfx(AssetsManager->GetProjectileVfxName(Proprietor->GetCharacterName(), EVfxType::FpsTrail), nullptr, GetSocketLocation("FpsProjectileSocket"), GetSocketRotation("FpsProjectileSocket"), Proprietor->GetVariation());
}

void AWeapon::P_ReloadAmmo(const int32 InOtherAmmoSource)
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
	P_UpdateAmmoTextHUD();
}

void AWeapon::SetupHandsOptimization() const
{
	HandsMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	HandsMesh->SetComponentTickEnabled(false);
	HandsMesh->bEnableUpdateRateOptimizations = true;
	HandsMesh->SkipUpdateOverlapsOptimEnabled = true;
	HandsMesh->SetAllowAnimCurveEvaluation(false);
	HandsMesh->bUseRefPoseOnInitAnim = true;
	HandsMesh->SetComponentTickInterval(1.f / 60.f);
}

void AWeapon::DisableComponentTick()
{
	if (!GetMesh()) return;
	GetMesh()->SetComponentTickEnabled(false);
}

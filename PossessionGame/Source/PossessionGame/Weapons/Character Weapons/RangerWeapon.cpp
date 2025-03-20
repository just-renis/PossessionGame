#include "RangerWeapon.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Enums/EVfxType.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/UpgradeManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/WeaponAnimInstance.h"

ARangerWeapon::ARangerWeapon()
{
	Crosshair = CreateDefaultSubobject<UStaticMeshComponent>("crosshair");
	Crosshair->SetupAttachment(RootComponent);
	Sounds.Add(ESound::RangerShoot, CreateDefaultSubobject<UAudioComponent>(TEXT("shootSound")));
	Sounds[ESound::RangerShoot]->SetAutoActivate(false);
	Sounds[ESound::RangerShoot]->SetupAttachment(Mesh);
	Sounds.Add(ESound::RangerReload, CreateDefaultSubobject<UAudioComponent>(TEXT("reloadSound")));
	Sounds[ESound::RangerReload]->SetAutoActivate(false);
	Sounds[ESound::RangerReload]->SetupAttachment(Mesh);
}

void ARangerWeapon::AI_Reload() { PlaySound(ESound::RangerReload); }

void ARangerWeapon::AI_Shoot()
{
	PlaySound(ESound::RangerShoot);
	Super::AI_Shoot();
	ShootProjectile(EPoolObject::RangerProjectile);
}

void ARangerWeapon::P_Shoot(bool bIsFirstWeapon)
{
	PlaySound(ESound::RangerShoot);
	HandsAnimInstance->PlayMontage(EHandsAnimState::Shoot);
	Super::P_Shoot(bIsFirstWeapon);
}

void ARangerWeapon::P_Reload(int32 OtherAmmoSource)
{
	PlaySound(ESound::RangerReload);
	HandsAnimInstance->PlayMontage(EHandsAnimState::Reload);
	if (AnimInstance) AnimInstance->PlayMontage(EWeaponAnimState::Reload);
	Super::P_Reload(OtherAmmoSource);
}

void ARangerWeapon::P_ApplyUpgradeOnSelection(const EUpgrade InUpgradeName)
{
	if (InUpgradeName == EUpgrade::RangerBaseStat)
	{
		const float PercentageDamageValue = GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(true, Proprietor->GetCharacterName(), EUpgradeAttribute::DamagePercentage);
		HitScanDamage += GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(true, Proprietor->GetCharacterName(), EUpgradeAttribute::FlatDamage);
		HitScanDamage += P_Data->GetHitScanDamage() * (PercentageDamageValue / 100.f);
	}
}

void ARangerWeapon::BeginPlay()
{
	Name = EWeapon::RangerWeapon;
	Super::BeginPlay();
	AnimInstance = Cast<UWeaponAnimInstance>(Mesh->GetAnimInstance());
	AnimInstance->SetWeaponName(Name);
	Crosshair->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, "CrosshairSocket");
	SetupHandsAnimInstance();
}

void ARangerWeapon::HitScanLogic()
{
	if (bIsZoomingIn)
	{
		FHitResult HitResult;
		const FVector StartLocation = Crosshair->GetComponentLocation();
		FVector EndLocation = StartLocation + Crosshair->GetForwardVector() * 15000.f;
		EndLocation += FVector(FMath::RandRange(-P_GetHitScanCalcSpread(), P_GetHitScanCalcSpread()), FMath::RandRange(-P_GetHitScanCalcSpread(), P_GetHitScanCalcSpread()), FMath::RandRange(-P_GetHitScanCalcSpread(), P_GetHitScanCalcSpread()));
		AssetsManager->SpawnOrLoadVfx(AssetsManager->GetVfxName(Proprietor->GetCharacterName(), EVfxType::FpsTrail), GetSocketLocation("ProjectileSocket"), GetSocketRotation("ProjectileSocket"));
		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility))
		{
			AssetsManager->SpawnOrLoadVfx(AssetsManager->GetVfxName(Proprietor->GetCharacterName(), EVfxType::Impact), HitResult.Location, HitResult.ImpactNormal.Rotation());
			if (HitResult.GetActor()->ActorHasTag("Enemy") || HitResult.GetActor()->ActorHasTag("Destructible")) DealPointDamage(HitResult);
		}
	}
	else Super::HitScanLogic();
}

void ARangerWeapon::P_ZoomToCamera(bool bIsZooming, FVector2D& MouseSensitivity)
{
	(void)DelegateManager->OnZoomToggle.ExecuteIfBound();
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	UKismetSystemLibrary::MoveComponentTo(HandsMesh, bIsZooming ? P_Data->GetAimLocation() : P_Data->GetFirstWeaponLocation(), HandsMesh->GetRelativeRotation(), false, false, 0.2f, true, EMoveComponentAction::Type::Move, LatentInfo);
	bIsZoomingIn = bIsZooming;
	HandsAnimInstance->SetIsZoomedIn(bIsZooming);
	bIsWeaponZoomingInMotion = bIsZooming;
	ZoomStartTime = bIsZooming ? -1.f : GetWorld()->GetTimeSeconds();
	MouseSensitivity += bIsZooming ? -FVector2D(0.7f, 0.7f) : FVector2D(0.7f, 0.7f);
}

void ARangerWeapon::ZoomLogic(TObjectPtr<UCameraComponent> Camera, float DeltaTime)
{
	if (bIsWeaponZoomingInMotion)
	{
		ZoomingTimer += DeltaTime;
		if (ZoomingTimer >= 0.1f)
		{
			ZoomStartTime = GetWorld()->GetTimeSeconds();
			bIsWeaponZoomingInMotion = false;
			ZoomingTimer = 0.f;
		}
	}
	else if (ZoomStartTime >= 0.f)
	{
		float InterpFactor = FMath::Clamp((GetWorld()->GetTimeSeconds() - ZoomStartTime) / 0.2f, 0.f, 1.f);
		Camera->SetFieldOfView(FMath::Lerp(Camera->FieldOfView, bIsZoomingIn ? 40.f : 90.f, InterpFactor));
		if (InterpFactor >= 1.f) ZoomStartTime = -1.f;
	}
}

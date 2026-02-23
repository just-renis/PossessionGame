#include "SpyWeapon.h"
#include "Camera/CameraComponent.h"
#include "PossessionGame/AI/Enemies/Characters/Spy.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Enums/EVfxType.h"
#include "PossessionGame/Projectiles/Throwables/BreacherBarrel.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Weapons/Animation Instances/Hands/SpyHandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/WeaponAnimInstance.h"

void ASpyWeapon::SetInvincibilityState(const bool bState)
{
	if (GetHandsAnimInstance()) Cast<USpyHandsAnimInstance>(GetHandsAnimInstance())->SetInvincibleState(bState);
	GetWorldTimerManager().SetTimer(ProcessInvincibilityMaterialHandle, this, bState ? &ASpyWeapon::ProcessInvincibilityMaterialActivation : &ASpyWeapon::ProcessInvincibilityMaterialDeactivation, 0.02f, true);
}

void ASpyWeapon::AI_Shoot()
{
	//PlaySound(ESound::TricksterShoot);
	Super::AI_Shoot();
	ShootProjectile(EPoolObject::SpyProjectile, false, NAME_None, true);
}

void ASpyWeapon::P_Shoot(const EPoolObject InProjectileName)
{
	HandsAnimInstance->PlayMontage(EHandsAnimState::Shoot);
	Super::P_Shoot(InProjectileName);
}

void ASpyWeapon::P_Reload()
{
	HandsAnimInstance->PlayMontage(EHandsAnimState::Reload);
	if (AnimInstance) AnimInstance->PlayMontage(EWeaponAnimState::Reload);
}

void ASpyWeapon::P_Setup()
{
	Super::P_Setup();
	Mesh->SetComponentTickEnabled(true);
}

void ASpyWeapon::BeginPlay()
{
	Name = EWeapon::SpyWeapon;
	Super::BeginPlay();
	AnimInstance = Cast<UWeaponAnimInstance>(Mesh->GetAnimInstance());
	AnimInstance->SetWeaponName(Name);
	if (!HandsMesh) return;
	MID_DefaultMaterial = HandsMesh->CreateDynamicMaterialInstance(0);
}

void ASpyWeapon::HitScanLogic()
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
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, Cast<ASpy>(Proprietor)->GetXrayState() ? ECC_GameTraceChannel3 : ECC_Visibility, QueryParams))
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

void ASpyWeapon::ProcessInvincibilityMaterialActivation()
{
	if (!MID_DefaultMaterial) return;
	MaterialEmissiveEffect += 0.1f;
	MaterialWidth += 0.02f;
	MaterialHue += 0.085f;
	MaterialColorStrength += 0.9f;
	MID_DefaultMaterial->SetScalarParameterValue(FName("Emissive Effect"), MaterialEmissiveEffect);
	MID_DefaultMaterial->SetScalarParameterValue(FName("Width"), MaterialWidth);
	MID_DefaultMaterial->SetScalarParameterValue(FName("Hue"), MaterialHue);
	MID_DefaultMaterial->SetScalarParameterValue(FName("Color Strength"), MaterialColorStrength);
	if (++MaterialChangeCountLoops >= 10)
	{
		GetWorldTimerManager().ClearTimer(ProcessInvincibilityMaterialHandle);
		MaterialChangeCountLoops = 0;
	}
}

void ASpyWeapon::ProcessInvincibilityMaterialDeactivation()
{
	if (!MID_DefaultMaterial) return;
	MaterialEmissiveEffect -= 0.1f;
	MaterialWidth -= 0.02f;
	MaterialHue -= 0.085f;
	MaterialColorStrength -= 0.9f;
	MID_DefaultMaterial->SetScalarParameterValue(FName("Emissive Effect"), MaterialEmissiveEffect);
	MID_DefaultMaterial->SetScalarParameterValue(FName("Width"), MaterialWidth);
	MID_DefaultMaterial->SetScalarParameterValue(FName("Hue"), MaterialHue);
	MID_DefaultMaterial->SetScalarParameterValue(FName("Color Strength"), MaterialColorStrength);
	if (++MaterialChangeCountLoops >= 10)
	{
		GetWorldTimerManager().ClearTimer(ProcessInvincibilityMaterialHandle);
		MaterialChangeCountLoops = 0;
	}
}

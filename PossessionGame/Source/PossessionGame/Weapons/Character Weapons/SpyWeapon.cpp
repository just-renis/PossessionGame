#include "SpyWeapon.h"
#include "Camera/CameraComponent.h"
#include "PossessionGame/AI/Enemies/Characters/Spy.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Enums/EVfxType.h"
#include "PossessionGame/Projectiles/Throwables/BreacherBarrel.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/WeaponAnimInstance.h"

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

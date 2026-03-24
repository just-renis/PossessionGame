#include "SpyWeapon.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Enemies/Characters/Spy.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Enums/EPlayerLevelUpgradeId.h"
#include "PossessionGame/Enums/EVfxType.h"
#include "PossessionGame/Projectiles/Throwables/BreacherBarrel.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/PlayerDataManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Weapons/Animation Instances/Hands/SpyHandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/WeaponAnimInstance.h"

void ASpyWeapon::SetInvincibilityState(const bool bState)
{
	if (GetHandsAnimInstance()) Cast<USpyHandsAnimInstance>(GetHandsAnimInstance())->SetInvincibleState(bState);
	GetWorldTimerManager().SetTimer(ProcessInvincibilityMaterialHandle, this, bState ? &ASpyWeapon::ProcessInvincibilityMaterialActivation : &ASpyWeapon::ProcessInvincibilityMaterialDeactivation, 0.02f, true);
}

ASpyWeapon::ASpyWeapon()
{
	ShootSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("ShootSfx"));
	ShootSfx->SetAutoActivate(false);
	ShootSfx->SetupAttachment(Mesh);
	Sounds.Add(ESound::SpyShoot, ShootSfx);
}

void ASpyWeapon::AI_Shoot()
{
	PlaySound(ESound::SpyShoot, true);
	Super::AI_Shoot();
	ShootProjectile(EPoolObject::SpyProjectile, false, NAME_None, true);
}

float ASpyWeapon::P_UpgradeDamageAppliance()
{
	if (Proprietor->IsBoostedAttacksAvailable())
	{
		Proprietor->SubtractBoostedAttack();
		return P_ReceiveHitScanDamage() * 0.3f;
	}
	return Super::P_UpgradeDamageAppliance();
}

void ASpyWeapon::P_Shoot(const EPoolObject InProjectileName)
{
	AssetsManager->PlaySound2D(ESound::SpyShoot);
	HandsAnimInstance->PlayMontage(EHandsAnimState::Shoot);
	Super::P_Shoot(InProjectileName);
}

void ASpyWeapon::P_Reload()
{
	AssetsManager->PlaySound2D(ESound::SpyReload, true);
	HandsAnimInstance->PlayMontage(EHandsAnimState::Reload);
	if (AnimInstance) AnimInstance->PlayMontage(EWeaponAnimState::Reload);
}

void ASpyWeapon::P_Setup()
{
	Super::P_Setup();
	Mesh->SetComponentTickEnabled(true);
}

void ASpyWeapon::P_DealDamageThroughWeakPoint(const FHitResult& InHitResult)
{
	const TObjectPtr<USaveSlot> SaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot();
	const bool bIsUpgradeSelected = SaveSlot->IsPlayerLevelUpgradeSelected(EPlayerLevelUpgradeId::FifthOptionB);
	const bool bIsUpgradeSelectedEightOptionA = SaveSlot->IsPlayerLevelUpgradeSelected(EPlayerLevelUpgradeId::EighthOptionA);
	
	const float AfterUpgradeApplianceDamage = DefaultHitScanDamage + P_UpgradeDamageAppliance();
	
	float AfterPlayerLevelAllGlobalDamage = AfterUpgradeApplianceDamage + P_ReceiveHitScanDamage() * GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetPlayerLevelAllGlobalDamageBonus();
	
	AfterPlayerLevelAllGlobalDamage += DefaultHitScanDamage * (bIsUpgradeSelected ? SaveSlot->GetPlayerLevel() * 0.05f : 0.f);
	
	AfterPlayerLevelAllGlobalDamage += DefaultHitScanDamage * GetGameInstance()->GetSubsystem<UPlayerDataManager>()->GetPoisonAreaStackBoost(); // Trickster Purple Variation Damage Boost.

	AfterPlayerLevelAllGlobalDamage *= WeakPointDamagePercentage;
	
	if (bIsUpgradeSelectedEightOptionA && FMath::RandRange(1, 100) < 26) AfterPlayerLevelAllGlobalDamage *= 2.f;
	
	const float RangeMultiplier = P_Data->GetDamageMultiplierFromRange(InHitResult.Distance * 0.01f);
	if (FMath::IsNearlyEqual(RangeMultiplier, 0.f, 0.01f)) return;
	const bool bIsCritical = InHitResult.BoneName.IsEqual(FName("head"));
	
	float CriticalDamageMultiplier = 1.f;
	if (bIsCritical) CriticalDamageMultiplier = P_GetCriticalDamage();
	CriticalDamageMultiplier += GetGameInstance()->GetSubsystem<UPlayerDataManager>()->GetRangerBlueVariationStackBoost(); // Ranger Blue Variation Critical Damage Boost.
	float FinalDamageOutput = AfterPlayerLevelAllGlobalDamage * CriticalDamageMultiplier * RangeMultiplier;
	P_ProcessFinalDamageOutput(FinalDamageOutput);
	UGameplayStatics::ApplyPointDamage(InHitResult.GetActor(), FinalDamageOutput, (InHitResult.Location - GetActorLocation()).GetSafeNormal(), InHitResult, nullptr, this, UDamageType::StaticClass());
}

void ASpyWeapon::P_ProcessFinalDamageOutput(float& OutFinalDamageOutput)
{
	if (!bReduceDamageThroughObjects) return;
	OutFinalDamageOutput *= ReduceDamageThroughObjectsPercentage;
	bReduceDamageThroughObjects = false;
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
	const bool XrayState = Cast<ASpy>(Proprietor)->GetXrayState();
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, XrayState ? ECC_GameTraceChannel3 : ECC_Visibility, QueryParams))
	{
		AssetsManager->SpawnVfx(AssetsManager->GetProjectileVfxName(Proprietor->GetCharacterName(), EVfxType::Impact), nullptr, HitResult.Location, HitResult.ImpactNormal.Rotation(), Proprietor->GetVariation());
		if (XrayState)
		{
			FHitResult NonXrayHitResult;
			if (GetWorld()->LineTraceSingleByChannel(NonXrayHitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams))
			{
				if (NonXrayHitResult.GetActor() != HitResult.GetActor()) bReduceDamageThroughObjects = true;
			}
			const TObjectPtr<UPrimitiveComponent> HitComp = HitResult.GetComponent();
			if (HitComp && HitComp->ComponentHasTag("WeakSpot"))
			{
				HitResult.TraceStart = StartLocation;
				P_DealDamageThroughWeakPoint(HitResult);
				return;
			}
		}
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

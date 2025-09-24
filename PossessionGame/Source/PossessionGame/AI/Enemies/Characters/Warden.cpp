#include "Warden.h"
#include "NiagaraComponent.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EStateTreeEvent.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"

AWarden::AWarden()
{
	BeamWeapon = CreateDefaultSubobject<USkeletalMeshComponent>("BeamWeapon");
}

void AWarden::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::WardenBlasterWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void AWarden::BeginPlay()
{
	CharacterName = ECharacterType::Warden;
	Super::BeginPlay();
	bCanShoot = true;
	BeamWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("BeamWeaponSocket"));
	AssetsManager->AttachVfx(EVfx::WardenBeamShot, [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed(); }, &BeamVfx, BeamWeapon, FName("BeamSocket"), true);
	GetWorldTimerManager().SetTimer(BeamTriggerHandle, this, &AWarden::TryTriggeringBeamShot, 1.f, true);
}

void AWarden::SetEmpEffect(const float InEffectDuration)
{
	switch (GetAI()->GetCurrentStateTreeEvent())
	{
		case EStateTreeEvent::WardenBeamShot: AI_StopAbility(0); break;
		default: ;
	}
	Super::SetEmpEffect(InEffectDuration);
}

void AWarden::TryTriggeringBeamShot()
{
	if (!CanAbilityBeExecuted(0) || GetIsAbilityInAction() || !AI) return;
	AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::WardenBeamShot);
}

void AWarden::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(BeamTriggerHandle);
	Super::EndPlay(EndPlayReason);
}

void AWarden::TriggerBeam() const
{
	if (!BeamVfx) return;
	const TObjectPtr<AActor> Player = AI->GetStateTreeData().GetTarget();
	if (!Player) return;
	// Beam calculation logic.
	const FVector Start = BeamWeapon->GetSocketLocation(FName("BeamSocket"));
	const FVector Direction = Player->GetActorLocation() - Start;
	float Distance = Direction.Size();
	// Beam blocking logic.
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	const FVector LaserEnd = Start + BeamWeapon->GetSocketRotation(FName("BeamSocket")).Vector() * 4000.f;
	FHitResult LaserHit;
	GetWorld()->LineTraceSingleByChannel(LaserHit, Start, LaserEnd, ECC_Visibility, QueryParams);
	if (LaserHit.bBlockingHit) Distance = LaserHit.Distance;
	// Beam collision setup.
	//Proprietor->GetLaserCollision()->SetWorldLocation(Start + LaserForward * Distance * 0.45f);
	//Proprietor->GetLaserCollision()->SetWorldRotation(NewRotation);
	//Proprietor->GetLaserCollision()->SetCapsuleSize(35.f, Distance * 0.5f);
	// Beam length vfx change.
	BeamVfx->SetVectorParameter(FName("Scale Factor 1"), FVector(Distance * DistanceMulti, 1.f, 1.f));
	BeamVfx->SetVectorParameter(FName("Scale Factor001"), FVector(Distance * DistanceMulti, 1.f, 1.f));
	BeamVfx->Activate(true);
}

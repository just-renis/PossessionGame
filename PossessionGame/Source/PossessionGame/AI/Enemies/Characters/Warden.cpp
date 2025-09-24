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

void AWarden::TriggerBeam() const { if (BeamVfx) BeamVfx->Activate(true); }

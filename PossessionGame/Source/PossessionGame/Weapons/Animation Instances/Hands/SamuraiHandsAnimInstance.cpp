#include "SamuraiHandsAnimInstance.h"
#include "NiagaraComponent.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Weapons/Character Weapons/SamuraiWeapon.h"

void USamuraiHandsAnimInstance::PlaySlashVfx()
{
	if (!SlashVfx) AssetsManager->AttachVfx(EVfx::PlayerSamuraiSlash, [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed(); }, &SlashVfx, GetSkelMeshComponent(), FName("RootSocket"));
	else SlashVfx->Activate(true);
}

void USamuraiHandsAnimInstance::SetProprietor(const TObjectPtr<AEnemy>& InProprietor)
{
	Super::SetProprietor(InProprietor);
	Weapon = Cast<ASamuraiWeapon>(Proprietor->GetWeapon());
}

void USamuraiHandsAnimInstance::OnMontageEnd(UAnimMontage* InMontage, const bool bIsInterrupted)
{
	Super::OnMontageEnd(InMontage, bIsInterrupted);
	if (!IsMontage(EHandsAnimState::QuickMelee, InMontage)) return;
	FTimerHandle AttackCooldownHandle;
	GetWorld()->GetTimerManager().SetTimer(AttackCooldownHandle, this, &USamuraiHandsAnimInstance::SetAttackCooldownFinished, AttackCooldown);
}

void USamuraiHandsAnimInstance::SetAttackCooldownFinished() const
{
	if (Weapon) Weapon->P_ChangeAttackState(true);
}

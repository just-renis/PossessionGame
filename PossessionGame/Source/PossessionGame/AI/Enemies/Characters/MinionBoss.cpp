#include "MinionBoss.h"
#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/TriggerSphere.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void AMinionBoss::AI_ToggleStompTrigger(const bool bState) const
{
	if (StompTriggerSphere)
	{
		StompTriggerSphere->GetCollisionComponent()->SetCollisionEnabled(bState ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
		StompTriggerSphere->SetActorHiddenInGame(!bState);
	}
}

void AMinionBoss::AI_SetupWeapon()
{
	for (int32 i = 0; i < 4; i++)
	{
		Weapons.Add(GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::MinionBossHandWeapon, GetMesh(), FName("AIWeaponSocket" + FString::FromInt(i + 1))));
		Weapons[i]->SetProprietor(this);
		Weapons[i]->PostProprietorSetup();
	}
	AI->SetShootCooldown(Weapons[0]->GetShootCooldown());
}

void AMinionBoss::AI_ShootLogic()
{
	if (!bIsActionInMotion && bCanShoot) AI_Shoot();
}

void AMinionBoss::AI_Shoot()
{
	Weapons[CurrentWeaponIndex++]->AI_Shoot();
	if (CurrentWeaponIndex >= 4) CurrentWeaponIndex = 0;
}

void AMinionBoss::AI_SetCanShoot(const bool bState)
{
	bCanShoot = bState;
	if (!bCanShoot) AI_SetShootPause();
}

void AMinionBoss::AI_CancelShooting()
{
	CurrentWeaponIndex = 0;
	if (!AnimInstance) return;
	AnimInstance->StopMontage(EEnemyAnimState::Shoot);
}

void AMinionBoss::AI_InitiateShooting() { AI_PlayShootMontage(); }

void AMinionBoss::AI_OnPlayerEnterStompTrigger(AActor* OverlappedActor, AActor* OtherActor)
{
	AI->GetStateTreeData().TriggerEvent("StateTree.MinionBoss.Stomp");
	AI_ToggleStompTrigger(false);
}

TObjectPtr<AWeapon>& AMinionBoss::GetWeapon() { return Weapons[0]; }

void AMinionBoss::BeginPlay()
{
	CharacterName = ECharacterType::MinionBoss;
	Super::BeginPlay();
	SetupStompTrigger();
}

void AMinionBoss::HandleDeactivation()
{
	for (int32 i = 0; i < Weapons.Num(); i++) Weapons[i]->DeactivateWeapon();
	Super::HandleDeactivation();
}

void AMinionBoss::DestroyThis()
{
	for (int32 i = 0; i < Weapons.Num(); i++) Weapons[i]->DestroyThis();
	Super::DestroyThis();
}

void AMinionBoss::SetupStompTrigger()
{
	StompTriggerSphere = GetWorld()->SpawnActor<ATriggerSphere>(ATriggerSphere::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	StompTriggerSphere->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	StompTriggerSphere->GetCollisionComponent()->SetCollisionProfileName("TriggerPlayer");
	StompTriggerSphere->GetCollisionComponent()->CanCharacterStepUpOn = ECB_No;
	StompTriggerSphere->GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Cast<USphereComponent>(StompTriggerSphere->GetCollisionComponent())->SetSphereRadius(StompTriggerRadiusInMetres * 100.f);
	StompTriggerSphere->OnActorBeginOverlap.AddDynamic(this, &AMinionBoss::AI_OnPlayerEnterStompTrigger);
}

void AMinionBoss::DestroyStompTriggerSphere() const
{
	if (StompTriggerSphere)
	{
		StompTriggerSphere->OnActorBeginOverlap.RemoveAll(this);
		StompTriggerSphere->Destroy();
	}
}

void AMinionBoss::InitiateDeath()
{
	DestroyStompTriggerSphere();
	Super::InitiateDeath();
}

#include "EMPGrenade.h"
#include "Components/AudioComponent.h"
#include "Engine/OverlapResult.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

AEMPGrenade::AEMPGrenade()
{
	Sounds.Add(ESound::TricksterEmpImpact, CreateDefaultSubobject<UAudioComponent>(TEXT("EmpImpactSound")));
	Sounds[ESound::TricksterEmpImpact]->SetAutoActivate(false);
	Sounds[ESound::TricksterEmpImpact]->SetupAttachment(Mesh);
}

void AEMPGrenade::P_SetVariationMaterial()
{
	if (!MID_Variation)
	{
		MID_Variation = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(1), this);
		Mesh->SetMaterial(1, MID_Variation);
		if (!MID_Variation) return;
	}
	ProjectileData.SetupVariationMaterial(MID_Variation);
}

void AEMPGrenade::PlayImpactSound() { PlaySound(ESound::TricksterEmpImpact); }

void AEMPGrenade::OnImpact(const FHitResult& Hit)
{
	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByProfile(Overlaps, Hit.Location, FQuat::Identity, FName("TriggerPlayer"), FCollisionShape::MakeSphere(EmpRadius));
	for (const FOverlapResult& Result : Overlaps)
	{
		TObjectPtr<AActor> Actor = Result.GetActor();
		if (!Actor || !Actor->ActorHasTag("AliveEnemy")) continue;
		if (const TObjectPtr<AEnemy> Target = Cast<AEnemy>(Actor)) if (!Target->GetIsHijacked()) Target->AI_ReceiveEmpEffect(EffectDuration);
	}
	Super::OnImpact(Hit);
}

void AEMPGrenade::Init(FProjectileData& InProjectileData, const FRotator& Rotation, const int32 InWhich)
{
	Super::Init(InProjectileData, Rotation, InWhich);
	const float EffectDurationIncrease = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::TricksterAbility2, EUpgradeAttribute::EffectDuration, ProjectileData.bIsPlayerProjectile);
	EffectDuration = ProjectileData.EffectDuration + ProjectileData.EffectDuration * EffectDurationIncrease * 0.01f;
}

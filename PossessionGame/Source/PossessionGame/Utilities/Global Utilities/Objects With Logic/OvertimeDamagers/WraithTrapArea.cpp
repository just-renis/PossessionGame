#include "WraithTrapArea.h"
#include "NiagaraComponent.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Data Assets/Base Data Assets/DamageOverTimeData.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Data Utilities/FNiagaraAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

void AWraithTrapArea::BeginPlay()
{
	Super::BeginPlay();
	ObjectName = EPoolObject::WraithTrapArea;
}

void AWraithTrapArea::DealOvertimeDamage()
{
	if (Targets.IsEmpty()) return;
	for (int32 i = 0; i < Targets.Num(); i++)
	{
		if (!Targets[i]) continue;
		Targets[i]->AddSlowingStack(EffectStrength);
	}
}

void AWraithTrapArea::Setup(const bool bInIsPlayerObject, const TObjectPtr<UDamageOverTimeData>& InDamageOverTimeData, const EDamageReason InDamageReason)
{
	Super::Setup(bInIsPlayerObject, InDamageOverTimeData, InDamageReason);
	TMap<FName, FNiagaraAttribute> Attributes;
	const float RadiusIncrease = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::WraithAbility2, EUpgradeAttribute::Size, true);
	const float CollisionRadius = DefaultRadius + DefaultRadius * RadiusIncrease * 0.01f;
	Attributes.Add(FName("Mesh Uniform Scale"), FNiagaraAttribute(true, CollisionRadius / DefaultRadius));
	SetupRadiusAndTimers(360.f, InDamageOverTimeData->GetAreaLifeSpan());
	if (!RadiusVfx)
	{
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::WraithTrap, nullptr, &RadiusVfx, RootComponent, NAME_None,
		false, FVector::ZeroVector, FRotator(-90.f, 0.f, 0.f), EAttachLocation::SnapToTarget, false, EVariation::Default, Attributes);
	}
	else
	{
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->UpdateVfxAttributes(RadiusVfx, Attributes);
		RadiusVfx->Activate(true);
	}
}

void AWraithTrapArea::RemoveTarget(const TObjectPtr<AEnemy>& InTarget)
{
	Super::RemoveTarget(InTarget);
	//InTarget
}

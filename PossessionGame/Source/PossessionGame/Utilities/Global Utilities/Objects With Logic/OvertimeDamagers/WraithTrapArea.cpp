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
	const float RadiusIncrease = 1.f + GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::WraithAbility2, EUpgradeAttribute::Size, true) * 0.01f;
	const float LifeSpanIncrease = 1.f + GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::WraithAbility2, EUpgradeAttribute::EffectDuration, true) * 0.01f;

	const float DefaultLifeSpan = InDamageOverTimeData->GetAreaLifeSpan();
	const float CollisionRadius = DefaultRadius * RadiusIncrease;
	const float LifeSpan = DefaultLifeSpan * LifeSpanIncrease;

	const float DifferenceRadiusDelta = DefaultRadius / DefaultVfxRadius;
	const float DifferenceLifetimeDelta = DefaultLifeSpan / DefaultVfxLifetime;

	const float AuraRadius = DefaultAuraRadius * RadiusIncrease * DifferenceRadiusDelta;
	const float FloorAuraRadius = DefaultFloorAuraRadius * RadiusIncrease * DifferenceRadiusDelta;
	const float SmokeRadius = DefaultSmokeRadius * RadiusIncrease * DifferenceRadiusDelta;
	const float SmokeCount = DefaultSmokeCount * RadiusIncrease * DifferenceRadiusDelta;
	const float BubbleCount = DefaultBubbleCount * RadiusIncrease * DifferenceRadiusDelta;
	const float GhostCount = DefaultGhostCount * RadiusIncrease * DifferenceRadiusDelta;
	const float GhostRadius = DefaultGhostRadius * RadiusIncrease * DifferenceRadiusDelta;
	const float SmokeLifetime = DefaultSmokeLifetime * LifeSpanIncrease * DifferenceLifetimeDelta;
	const float BubbleLifetime = DefaultBubbleLifetime * LifeSpanIncrease * DifferenceLifetimeDelta;
	const float GhostLifetime = DefaultGhostLifetime * LifeSpanIncrease * DifferenceLifetimeDelta;
	const float BaseLifeSpan = LifeSpan * DifferenceLifetimeDelta;
	SetupRadiusAndTimers(CollisionRadius * CollisionScalingOffsetSizeMultiplier, LifeSpan);
	
	Attributes.Add(FName("BubbleRadius"), FNiagaraAttribute(EUserParamType::Float, CollisionRadius * DifferenceRadiusDelta));
	Attributes.Add(FName("AuraRadius"), FNiagaraAttribute(EUserParamType::Vector, FVector(AuraRadius, AuraRadius, 0.25f)));
	Attributes.Add(FName("FloorAuraRadius"), FNiagaraAttribute(EUserParamType::Float, FloorAuraRadius));
	Attributes.Add(FName("SmokeRadius"), FNiagaraAttribute(EUserParamType::Float, SmokeRadius));
	Attributes.Add(FName("SmokeCount"), FNiagaraAttribute(EUserParamType::Float, SmokeCount));
	Attributes.Add(FName("BubbleCount"), FNiagaraAttribute(EUserParamType::Float, BubbleCount));
	Attributes.Add(FName("GhostCount"), FNiagaraAttribute(EUserParamType::Float, GhostCount));
	Attributes.Add(FName("GhostRadius"), FNiagaraAttribute(EUserParamType::Float, GhostRadius));
	
	Attributes.Add(FName("AuraLifetime"), FNiagaraAttribute(EUserParamType::Float, BaseLifeSpan));
	Attributes.Add(FName("FloorAuraLifetime"), FNiagaraAttribute(EUserParamType::Float, BaseLifeSpan));
	Attributes.Add(FName("SmokeLifetime"), FNiagaraAttribute(EUserParamType::Float, SmokeLifetime));
	Attributes.Add(FName("BubbleLifetime"), FNiagaraAttribute(EUserParamType::Float, BubbleLifetime));
	Attributes.Add(FName("GhostLifetime"), FNiagaraAttribute(EUserParamType::Float, GhostLifetime));
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

void AWraithTrapArea::AddTarget(const TObjectPtr<AEnemy>& InTarget)
{
	Super::AddTarget(InTarget);
	InTarget->AddActiveWraithTrap();
}

void AWraithTrapArea::RemoveTarget(const TObjectPtr<AEnemy>& InTarget)
{
	Super::RemoveTarget(InTarget);
	InTarget->RemoveActiveWraithTrap();
}

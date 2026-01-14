#include "WraithMolotovArea.h"
#include "NiagaraComponent.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Data Assets/Base Data Assets/DamageOverTimeData.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Data Utilities/FNiagaraAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

void AWraithMolotovArea::BeginPlay()
{
	Super::BeginPlay();
	ObjectName = EPoolObject::WraithMolotovArea;
}

void AWraithMolotovArea::DealOvertimeDamage()
{
	if (Targets.IsEmpty()) return;
	for (int32 i = 0; i < Targets.Num(); i++)
	{
		if (!Targets[i]) continue;
		Targets[i]->SetupBurningState(Damage, EffectTick, DamageTick);
	}
}

void AWraithMolotovArea::Setup(const bool bInIsPlayerObject, const TObjectPtr<UDamageOverTimeData>& InDamageOverTimeData, const EDamageReason InDamageReason)
{
	Super::Setup(bInIsPlayerObject, InDamageOverTimeData, InDamageReason);
	TMap<FName, FNiagaraAttribute> Attributes;
	const float RadiusIncrease = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::WraithAbility1, EUpgradeAttribute::Size, true) * 0.01f;
	float LifeSpanIncrease = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::WraithAbility1, EUpgradeAttribute::EffectDuration, true) * 0.01f;
	LifeSpanIncrease += TEST;
	const float DefaultLifeSpan = InDamageOverTimeData->GetAreaLifeSpan();
	
	const float CollisionRadius = DefaultRadius + DefaultRadius * RadiusIncrease;
	const float LifeSpan = DefaultLifeSpan + DefaultLifeSpan * LifeSpanIncrease;
	
	const float FloorAuraRadius = DefaultFloorAuraRadius + DefaultFloorAuraRadius * RadiusIncrease;
	const float BubblesRadius = DefaultBubblesRadius + DefaultBubblesRadius * RadiusIncrease;
	const float FlamesAmount = DefaultFlamesAmount + DefaultFlamesAmount * RadiusIncrease;
	const float AuraLifetime = DefaultAuraLifetime + DefaultAuraLifetime * LifeSpanIncrease;
	const float FlameLifetime = DefaultFlameLifetime + DefaultFlameLifetime * LifeSpanIncrease;
	const float BubblesLifetime = DefaultBubblesLifetime + DefaultBubblesLifetime * LifeSpanIncrease;
	SetupRadiusAndTimers(CollisionRadius, LifeSpan);
	
	Attributes.Add(FName("FlameRadius"), FNiagaraAttribute(true, CollisionRadius));
	Attributes.Add(FName("FloorAuraRadius"), FNiagaraAttribute(true, FloorAuraRadius));
	Attributes.Add(FName("BubblesRadius"), FNiagaraAttribute(true, BubblesRadius));
	Attributes.Add(FName("FlamesAmount"), FNiagaraAttribute(true, FlamesAmount));
	//Attributes.Add(FName("AuraRadius"), FNiagaraAttribute(true, FlamesAmount));
	Attributes.Add(FName("FloorAuraLifetime"), FNiagaraAttribute(true, LifeSpan));
	Attributes.Add(FName("AuraLifetime"), FNiagaraAttribute(true, AuraLifetime));
	Attributes.Add(FName("FlameLifetime"), FNiagaraAttribute(true, FlameLifetime));
	Attributes.Add(FName("BubblesLifetime"), FNiagaraAttribute(true, BubblesLifetime));
	if (!RadiusVfx)
	{
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::WraithMolotovRadius, nullptr, &RadiusVfx, RootComponent, NAME_None,
		false, FVector::ZeroVector, FRotator(-90.f, 0.f, 0.f), EAttachLocation::SnapToTarget, false, EVariation::Default, Attributes);
	}
	else
	{
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->UpdateVfxAttributes(RadiusVfx, Attributes);
		RadiusVfx->Activate(true);
	}
}

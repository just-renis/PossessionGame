#include "WraithMolotovArea.h"
#include "NiagaraComponent.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Data Assets/Base Data Assets/DamageOverTimeData.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Data Utilities/FNiagaraAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
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
	const TObjectPtr<UDelegateManager> DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	for (int32 i = 0; i < Targets.Num(); i++)
	{
		if (!Targets[i]) continue;
		Damage = DefaultDamage + GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::WraithAbility1, EUpgradeAttribute::FlatDamage, true);
		DamageTick = DefaultDamageTick - GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::WraithAbility1, EUpgradeAttribute::OverTimeTick, true);
		float BurnExplosionBoostPercentage = 1.f;
		if (DelegateManager->RequestSpecialUpgradeAvailability.IsBound() && DelegateManager->RequestSpecialUpgradeAvailability.Execute(EUpgrade::WraithAbility1, EUpgradeAttribute::Special1))
		{
			BurnExplosionBoostPercentage += GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetPlayerLevelAllGlobalDamageBonus();
			BurnExplosionBoostPercentage += GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetSecretBoost(EVariation::SecretCrateRewardDamage) * 0.01f;
		}
		Targets[i]->SetupBurningState(Damage, EffectTick, DamageTick, BurnExplosionBoostPercentage);
	}
}

void AWraithMolotovArea::Setup(const bool bInIsPlayerObject, const TObjectPtr<UDamageOverTimeData>& InDamageOverTimeData, const EDamageReason InDamageReason)
{
	Super::Setup(bInIsPlayerObject, InDamageOverTimeData, InDamageReason);
	TMap<FName, FNiagaraAttribute> Attributes;
	const float RadiusIncrease = 1.f + GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::WraithAbility1, EUpgradeAttribute::Size, true) * 0.01f;
	const float LifeSpanIncrease = 1.f + GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::WraithAbility1, EUpgradeAttribute::EffectDuration, true) * 0.01f;

	const float DefaultLifeSpan = InDamageOverTimeData->GetAreaLifeSpan();
	const float CollisionRadius = DefaultRadius * RadiusIncrease;

	const float DifferenceRadiusDelta = DefaultRadius / DefaultVfxRadius;
	const float DifferenceLifetimeDelta = DefaultLifeSpan / DefaultVfxLifetime;
	
	const float FloorAuraRadius = DefaultFloorAuraRadius * RadiusIncrease * DifferenceRadiusDelta;
	const float BubblesRadius = DefaultBubblesRadius * RadiusIncrease * DifferenceRadiusDelta;
	const float FlamesAmount = DefaultFlamesAmount * RadiusIncrease * DifferenceRadiusDelta;
	const float AuraRadius = DefaultAuraRadius * RadiusIncrease * DifferenceRadiusDelta;
	const float AuraLifetime = DefaultAuraLifetime * LifeSpanIncrease * DifferenceLifetimeDelta;
	const float FlameLifetime = DefaultFlameLifetime * LifeSpanIncrease * DifferenceLifetimeDelta;
	const float BubblesLifetime = DefaultBubblesLifetime * LifeSpanIncrease * DifferenceLifetimeDelta;
	const float FloorAuraLifetime = DefaultFloorAuraLifetime * LifeSpanIncrease * DifferenceLifetimeDelta;
	SetupRadiusAndTimers(CollisionRadius * CollisionScalingOffsetSizeMultiplier, DefaultLifeSpan * LifeSpanIncrease);
	
	Attributes.Add(FName("FlameRadius"), FNiagaraAttribute(EUserParamType::Float, CollisionRadius));
	Attributes.Add(FName("FloorAuraRadius"), FNiagaraAttribute(EUserParamType::Float, FloorAuraRadius));
	Attributes.Add(FName("BubblesRadius"), FNiagaraAttribute(EUserParamType::Float, BubblesRadius));
	Attributes.Add(FName("FlamesAmount"), FNiagaraAttribute(EUserParamType::Float, FlamesAmount));
	Attributes.Add(FName("AuraRadius"), FNiagaraAttribute(EUserParamType::Vector, FVector(AuraRadius, AuraRadius, 0.25f)));
	Attributes.Add(FName("FloorAuraLifetime"), FNiagaraAttribute(EUserParamType::Float, FloorAuraLifetime));
	Attributes.Add(FName("AuraLifetime"), FNiagaraAttribute(EUserParamType::Float, AuraLifetime));
	Attributes.Add(FName("FlameLifetime"), FNiagaraAttribute(EUserParamType::Float, FlameLifetime));
	Attributes.Add(FName("BubblesLifetime"), FNiagaraAttribute(EUserParamType::Float, BubblesLifetime));
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

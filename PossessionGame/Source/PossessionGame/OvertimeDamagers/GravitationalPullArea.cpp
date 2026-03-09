#include "GravitationalPullArea.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Data Assets/Base Data Assets/DamageOverTimeData.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EVfx.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

AGravitationalPullArea::AGravitationalPullArea()
{
	PassiveSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("PassiveSfx"));
	PassiveSfx->SetAutoActivate(false);
	PassiveSfx->SetupAttachment(RootComponent);
	Sounds.Add(ESound::TankGravitationalBallActive, PassiveSfx);
}

void AGravitationalPullArea::BeginPlay()
{
	Super::BeginPlay();
	ObjectName = EPoolObject::TankGravitationalPullArea;
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(PassiveSfx, TFunction<bool()>(), ESound::TankGravitationalBallActive);
}

void AGravitationalPullArea::DealOvertimeDamage()
{
	if (Targets.IsEmpty()) return;
	for (int32 i = 0; i < Targets.Num(); i++)
	{
		if (!Targets[i]) continue;
		Targets[i]->Pull(GetActorLocation(), EffectStrength);
	}
}

void AGravitationalPullArea::Disable(const bool bHide)
{
	if (PassiveSfx) PassiveSfx->Deactivate();
	if (ItselfVfx) ItselfVfx->DeactivateImmediate();
	Super::Disable(bHide);
}

void AGravitationalPullArea::Setup(const bool bInIsPlayerObject, const TObjectPtr<UDamageOverTimeData>& InDamageOverTimeData, const EDamageReason InDamageReason)
{
	Super::Setup(bInIsPlayerObject, InDamageOverTimeData, InDamageReason);
	const float LifeSpanIncrease = 1.f + GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::TankAbility2, EUpgradeAttribute::EffectDuration, bInIsPlayerObject) * 0.01f;
	const float ModifiedLifeSpan = InDamageOverTimeData->GetAreaLifeSpan() * LifeSpanIncrease;
	if (PassiveSfx) PassiveSfx->Activate();
	if (!ItselfVfx) GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::GravitationalPull, [this]() -> bool { return GetCollisionComponent()->GetVisibleFlag(); }, &ItselfVfx, GetCollisionComponent());
	else
	{
		ItselfVfx->SetFloatParameter(FName("Loop Duration"), ModifiedLifeSpan);
		ItselfVfx->Activate(true);
	}
	const float SizeIncrease = 1.f + GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::TankAbility2, EUpgradeAttribute::Size, bInIsPlayerObject) * 0.01f;
	SetupRadiusAndTimers(InDamageOverTimeData->GetAreaSize() * SizeIncrease, ModifiedLifeSpan);
}

void AGravitationalPullArea::SetupEffectStrength(const TObjectPtr<UDamageOverTimeData>& InDamageOverTimeData)
{
	const float EffectStrengthIncrease = 1.f + GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::TankAbility2, EUpgradeAttribute::EffectStrength, bIsPlayer) * 0.01f;
	EffectStrength = InDamageOverTimeData->GetEffectStrength() * EffectStrengthIncrease;
}

void AGravitationalPullArea::PlaySound(const ESound InSoundName)
{
	Sounds[InSoundName]->Sound ? Sounds[InSoundName]->Play() : GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(Sounds[InSoundName], TFunction<bool()>(), InSoundName);
}

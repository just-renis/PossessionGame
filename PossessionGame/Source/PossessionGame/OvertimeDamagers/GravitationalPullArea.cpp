#include "GravitationalPullArea.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Data Assets/Base Data Assets/DamageOverTimeData.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EVfx.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
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
	const TObjectPtr<USaveSlotManager> SaveSlotManager = GetGameInstance()->GetSubsystem<USaveSlotManager>();
	//if (SaveSlotManager->GetRunSaveSlot()->IsSpecialUpgradeAvailable(EUpgrade::TankAbility2, EUpgradeAttribute::Special1))
//	{
		const float AreaRadius = GetCollisionComponent()->GetScaledSphereRadius();
		const float LegendaryExplosionDamage = DefaultLegendaryExplosionDamage + DefaultLegendaryExplosionDamage * SaveSlotManager->GetSaveSlot()->GetPlayerLevelAllGlobalDamageBonus();
		UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), LegendaryExplosionDamage, LegendaryExplosionDamage * 0.25f, GetActorLocation(), AreaRadius * 0.25f,
		AreaRadius, 1.f, UDamageType::StaticClass(), TArray<AActor*>(), this, Proprietor->GetController(), ECC_GameTraceChannel4);
		TArray<FOverlapResult> Overlaps;
		GetWorld()->OverlapMultiByProfile(Overlaps, GetActorLocation(), FQuat::Identity, FName("TriggerPlayer"), FCollisionShape::MakeSphere(AreaRadius));
		for (const FOverlapResult& Result : Overlaps)
		{
			TObjectPtr<AActor> Actor = Result.GetActor();
			if (!Actor) continue;
			ACharacter* Character = Cast<ACharacter>(Result.GetActor());
			if (!Character) continue;
			FVector Direction = Character->GetActorLocation() - GetActorLocation();
			Direction.Z *= 0.2f;
			Direction.Normalize();
			Character->LaunchCharacter(Direction * KnockbackStrength, true, false);
		}
//	}
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

void AGravitationalPullArea::InitWithProprietor(const TObjectPtr<AEnemy> InProprietor)
{
	Super::InitWithProprietor(InProprietor);
	Proprietor = InProprietor;
}

void AGravitationalPullArea::PlaySound(const ESound InSoundName)
{
	Sounds[InSoundName]->Sound ? Sounds[InSoundName]->Play() : GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(Sounds[InSoundName], TFunction<bool()>(), InSoundName);
}

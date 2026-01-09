#include "WraithMolotovArea.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
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
		UGameplayStatics::ApplyDamage(Targets[i], Damage, nullptr, this, UDamageType::StaticClass());
	}
}

void AWraithMolotovArea::Setup(const bool bInIsPlayerObject, const float InDamage, const float InDamageTick, const float InLifeSpan, const float InRadiusSize, const EDamageReason InDamageReason, const FRotator& InRotation)
{
	Super::Setup(bInIsPlayerObject, InDamage, InDamageTick, InLifeSpan, InRadiusSize, InDamageReason, InRotation);
	TMap<FName, FNiagaraAttribute> Attributes;
	const float RadiusIncrease = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::WraithAbility1, EUpgradeAttribute::Size, true);
	const float CollisionRadius = DefaultRadius + DefaultRadius * RadiusIncrease * 0.01f;
	Attributes.Add(FName("Mesh Uniform Scale"), FNiagaraAttribute(true, CollisionRadius / DefaultRadius));
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

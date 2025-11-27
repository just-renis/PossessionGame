#include "BreacherDynamite.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Data Assets/Base Data Assets/ThrowableData.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

ABreacherDynamite::ABreacherDynamite()
{
	ExplodeSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("ExplodeSfx"));
	ExplodeSfx->SetAutoActivate(false);
	ExplodeSfx->SetupAttachment(Mesh);
	StickSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("StickSfx"));
	StickSfx->SetAutoActivate(false);
	StickSfx->SetupAttachment(Mesh);
	Sounds.Add(ESound::BreacherDynamiteExplosion, ExplodeSfx);
	Sounds.Add(ESound::BreacherDynamiteStick, StickSfx);
}

void ABreacherDynamite::Explode()
{
	bInExplosionProcess = true;
	const TObjectPtr<URunSaveSlot> RunSaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot();
	const float RadiusIncrease = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::BreacherAbility3, EUpgradeAttribute::Size, true);
	const float DamageFromUpgrades = RunSaveSlot->GetUpgradeAttributeValues(EUpgrade::BreacherAbility3, EUpgradeAttribute::FlatDamage, true);
	float NormalDamage = Data->GetDamage() + DamageFromUpgrades;
	float MinimumDamage = Data->GetRadiusMinimumDamage() + DamageFromUpgrades;
	const float InnerRadius = Data->GetDamageInnerRadius() + Data->GetDamageInnerRadius() * RadiusIncrease * 0.01f;
	const float OuterRadius = Data->GetDamageOuterRadius() + Data->GetDamageOuterRadius() * RadiusIncrease * 0.01f;
	const float LevelDamageBoost = Data->GetDamage() * GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetLevelDamageBoost() * 0.01f;
	NormalDamage += LevelDamageBoost;
	MinimumDamage += LevelDamageBoost;
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), NormalDamage, MinimumDamage, GetActorLocation(), InnerRadius, OuterRadius,
	Data->GetDamageFalloff(), UDamageType::StaticClass(), TArray{ Proprietor }, this, nullptr, ECC_GameTraceChannel4);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->SpawnVfx(EVfx::BreacherDynamiteExplosion, nullptr, GetActorLocation());
	PlaySound(ESound::BreacherDynamiteExplosion);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnDetonateDynamites.RemoveAll(this);
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestObjectWithCapRemoval.ExecuteIfBound(EPoolObject::BreacherDynamite, this);
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::BreacherDynamite, this);
	bInExplosionProcess = false;
}

void ABreacherDynamite::Throw()
{
	Super::Throw();
	GetWorldTimerManager().SetTimer(StickHandle, this, &ABreacherDynamite::PerformStickTrace, 0.05f, true);
}

void ABreacherDynamite::Init(const bool bInIsPlayerObject)
{
	Super::Init(bInIsPlayerObject);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnDetonateDynamites.AddUObject(this, &ABreacherDynamite::Explode);
}

void ABreacherDynamite::PerformStickTrace()
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(Proprietor);
	for (int32 i = 0; i < 4; i++)
	{
		const FVector Position = Mesh->GetSocketLocation(FName("StickingBoundsSocket" + FString::FromInt(i)));
		const FQuat Rotation = Mesh->GetSocketQuaternion(FName("StickingBoundsSocket" + FString::FromInt(i)));
		if (GetWorld()->SweepSingleByChannel(Hit, Position, Position, Rotation, ECC_WorldStatic, FCollisionShape::MakeBox(StickingBounds), Params))
		{
			Mesh->SetSimulatePhysics(false);
			GetWorldTimerManager().ClearTimer(StickHandle);
		}
	}
}

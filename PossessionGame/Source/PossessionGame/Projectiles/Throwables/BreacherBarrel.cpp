#include "BreacherBarrel.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Data Assets/Base Data Assets/ThrowableData.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

void ABreacherBarrel::Explode()
{
	const TObjectPtr<URunSaveSlot> RunSaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot();
	const float RadiusIncrease = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::BreacherAbility1, EUpgradeAttribute::Size, true);
	const float DamageIncrease = Data->GetDamage() * RunSaveSlot->GetUpgradeAttributeValues(EUpgrade::BreacherAbility1, EUpgradeAttribute::DamagePercentage, true) * 0.01f;
	float NormalDamage = Data->GetDamage() + DamageIncrease;
	float MinimumDamage = Data->GetRadiusMinimumDamage() + DamageIncrease;
	const float InnerRadius = Data->GetDamageInnerRadius() + Data->GetDamageInnerRadius() * RadiusIncrease * 0.01f;
	const float OuterRadius = Data->GetDamageOuterRadius() + Data->GetDamageOuterRadius() * RadiusIncrease * 0.01f;
	const float LevelDamageBoost = Data->GetDamage() * GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetLevelDamageIncrease() * 0.01f;
	NormalDamage += LevelDamageBoost;
	MinimumDamage += LevelDamageBoost;
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), NormalDamage, MinimumDamage, GetActorLocation(), InnerRadius, OuterRadius,
	Data->GetDamageFalloff(), UDamageType::StaticClass(), TArray{ Proprietor }, this, nullptr, ECC_GameTraceChannel4);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->SpawnVfx(EVfx::BreacherBarrelExplosion, nullptr, GetActorLocation());
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::BreacherBarrel, this);
	PlaySound(ESound::BreacherBarrelExplosion);
}

void ABreacherBarrel::Disable(const bool bHide)
{
	Mesh->SetSimulatePhysics(false);
	Super::Disable(bHide);
}

ABreacherBarrel::ABreacherBarrel()
{
	ExplodeSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("ExplodeSfx"));
	ExplodeSfx->SetAutoActivate(false);
	ExplodeSfx->SetupAttachment(Mesh);
	Sounds.Add(ESound::BreacherBarrelExplosion, ExplodeSfx);
}

void ABreacherBarrel::BeginPlay()
{
	Super::BeginPlay();
	Tags.Add(FName("BreacherBarrel"));
}

void ABreacherBarrel::PlaySound(const ESound InSoundName)
{
	Sounds[InSoundName]->Sound ? Sounds[InSoundName]->Play() :
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(Sounds[InSoundName], TFunction<bool()>(), InSoundName);
}

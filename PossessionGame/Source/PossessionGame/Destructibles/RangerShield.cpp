#include "RangerShield.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Data Assets/Base Data Assets/DestructibleData.h"
#include "PossessionGame/Enums/EAbility.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/UpgradeManager.h"

ARangerShield::ARangerShield()
{
	Sounds.Add(ESound::RangerDeployShield, CreateDefaultSubobject<UAudioComponent>(TEXT("deploySound")));
	Sounds[ESound::RangerDeployShield]->SetAutoActivate(false);
	Sounds[ESound::RangerDeployShield]->SetupAttachment(Mesh);
	Sounds.Add(ESound::RangerDissolveShield, CreateDefaultSubobject<UAudioComponent>(TEXT("dissolveSound")));
	Sounds[ESound::RangerDissolveShield]->SetAutoActivate(false);
	Sounds[ESound::RangerDissolveShield]->SetupAttachment(Mesh);
}

void ARangerShield::BeginPlay()
{
	Super::BeginPlay();
	M_ForceField = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(0), this);
	Mesh->SetMaterial(0, M_ForceField);
	M_Shield = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(1), this);
	Mesh->SetMaterial(1, M_Shield);
}

void ARangerShield::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(MoveShieldHandle);
	GetWorldTimerManager().ClearTimer(DissolveHandle);
	Super::EndPlay(EndPlayReason);
}

void ARangerShield::Init(bool bInIsPlayerObject)
{
	ShieldMaterialValueChanger = 0;
	const float SizeIncrease = GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(bInIsPlayerObject, EAbility::RangerAbility2, EUpgradeAttribute::Size);
	SetActorScale3D(FVector(GetActorScale3D().X + GetActorScale3D().X * SizeIncrease * 0.01f, GetActorScale3D().Y, GetActorScale3D().Z + GetActorScale3D().Z * SizeIncrease * 0.01f));
	PlaySound(ESound::RangerDeployShield);
	UpdateShieldMaterialValues(0.f, 0.7f);
	Super::Init(bInIsPlayerObject);
	GetWorldTimerManager().SetTimer(MoveShieldHandle, this, &ARangerShield::MoveShield, 0.05f, true);
}

void ARangerShield::DestroyThis()
{
	SetActorScale3D(FVector(1.f, 1.f, 1.f));
	SetActorEnableCollision(false);
	ShieldMaterialValueChanger = 0;
	PlaySound(ESound::RangerDissolveShield);
	GetWorldTimerManager().SetTimer(DissolveHandle, this, &ARangerShield::DissolveShield, 0.05f, true);
}

void ARangerShield::UpdateShieldMaterialValues(const float InOpacity, const float InProgress) const
{
	M_ForceField->SetScalarParameterValue("Opacity", InOpacity);
	M_Shield->SetScalarParameterValue("Progress", InProgress);
}

void ARangerShield::PlaySound(const ESound InSoundName)
{
	Sounds[InSoundName]->Sound ? Sounds[InSoundName]->Play() : GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(Sounds[InSoundName], nullptr, InSoundName);
}

void ARangerShield::MoveShield()
{
	if (++ShieldMaterialValueChanger >= 10) GetWorldTimerManager().ClearTimer(MoveShieldHandle);
	SetActorLocation(GetActorLocation() + Proprietor->GetActorForwardVector() * 10.f);
	UpdateShieldMaterialValues(0.f + ShieldMaterialValueChanger * 0.05f, 0.7f - ShieldMaterialValueChanger * 0.03f);
}

void ARangerShield::DissolveShield()
{
	if (++ShieldMaterialValueChanger >= 10)
	{
		GetWorldTimerManager().ClearTimer(DissolveHandle);
		Super::DestroyThis();
	}
	UpdateShieldMaterialValues(0.5f - ShieldMaterialValueChanger * 0.05f, 0.4f + ShieldMaterialValueChanger * 0.03f);
}

void ARangerShield::SetLifeSpan()
{
	const float EffectDurationIncrease = GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(bIsPlayerObject, EAbility::RangerAbility2, EUpgradeAttribute::EffectDuration);
	LifeSpan = Data->GetLifeSpan() + Data->GetLifeSpan() * EffectDurationIncrease * 0.01f;
	Super::SetLifeSpan();
}

void ARangerShield::SetUpgradesApplication()
{
	Hp = Data->GetHp() + GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(bIsPlayerObject, EAbility::RangerAbility2, EUpgradeAttribute::FlatHealth);
	const float ReductionIncrease = GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(bIsPlayerObject, EAbility::RangerAbility2, EUpgradeAttribute::ReduceDamageTaken);
	DamageReduction = DamageReduction / (1.f + ReductionIncrease * 0.01f);
	UE_LOG(LogTemp, Warning, TEXT("RANGER SHIELD TotalDamageReduction: %f"), DamageReduction);
}

#include "RangerShield.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Data Assets/Base Data Assets/DestructibleData.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PlayerDataManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

ARangerShield::ARangerShield()
{
	Sounds.Add(ESound::RangerDeployShield, CreateDefaultSubobject<UAudioComponent>(TEXT("deploySound")));
	Sounds[ESound::RangerDeployShield]->SetAutoActivate(false);
	Sounds[ESound::RangerDeployShield]->SetupAttachment(Mesh);
	Sounds.Add(ESound::RangerDissolveShield, CreateDefaultSubobject<UAudioComponent>(TEXT("dissolveSound")));
	Sounds[ESound::RangerDissolveShield]->SetAutoActivate(false);
	Sounds[ESound::RangerDissolveShield]->SetupAttachment(Mesh);
}

float ARangerShield::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsPlayerObject && GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestSpecialUpgradeAvailability.Execute(EUpgrade::RangerAbility2, EUpgradeAttribute::Special1))
	{
		UE_LOG(LogTemp, Error, TEXT("ADDED STACK"));
		GetGameInstance()->GetSubsystem<UPlayerDataManager>()->AddRangerShieldSpecialStack();
	}
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
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
	const float SizeIncrease = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::RangerAbility2, EUpgradeAttribute::Size, bInIsPlayerObject);
	SetActorScale3D(FVector(GetActorScale3D().X + GetActorScale3D().X * SizeIncrease * 0.01f, GetActorScale3D().Y, GetActorScale3D().Z + GetActorScale3D().Z * SizeIncrease * 0.01f));
	if (bInIsPlayerObject) GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::RangerDeployShield);
	else PlaySound(ESound::RangerDeployShield);
	UpdateShieldMaterialValues(0.f, 0.7f);
	Super::Init(bInIsPlayerObject);
	GetWorldTimerManager().SetTimer(MoveShieldHandle, this, &ARangerShield::MoveShield, 0.05f, true);
}

void ARangerShield::DestroyThis()
{
	SetActorScale3D(FVector(1.f));
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
	Sounds[InSoundName]->Sound ? Sounds[InSoundName]->Play() : GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(Sounds[InSoundName], nullptr, InSoundName);
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

void ARangerShield::SetThisLifeSpan()
{
	const float EffectDurationIncrease = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::RangerAbility2, EUpgradeAttribute::EffectDuration, bIsPlayerObject);
	LifeSpan = Data->GetLifeSpan() + Data->GetLifeSpan() * EffectDurationIncrease * 0.01f;
	Super::SetThisLifeSpan();
}

void ARangerShield::SetUpgradesApplication()
{
	const TObjectPtr<URunSaveSlot> RunSaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot();
	Hp = Data->GetHp() + RunSaveSlot->GetUpgradeAttributeValues(EUpgrade::RangerAbility2, EUpgradeAttribute::FlatHealth, bIsPlayerObject);
	const float ReductionIncrease = RunSaveSlot->GetUpgradeAttributeValues(EUpgrade::RangerAbility2, EUpgradeAttribute::ReduceDamageTaken, bIsPlayerObject);
	DamageReduction = 1.f / (1.f + ReductionIncrease * 0.01f);
}

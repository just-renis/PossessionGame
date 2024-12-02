#include "RangerShield.h"
#include "Components/AudioComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

ARangerShield::ARangerShield()
{
	Sounds.Add(ESound::RangerDeployShield, CreateDefaultSubobject<UAudioComponent>(TEXT("deploySound")));
	Sounds[ESound::RangerDeployShield]->SetAutoActivate(false);
	Sounds.Add(ESound::RangerDissolveShield, CreateDefaultSubobject<UAudioComponent>(TEXT("dissolveSound")));
	Sounds[ESound::RangerDissolveShield]->SetAutoActivate(false);
}

void ARangerShield::BeginPlay()
{
	Super::BeginPlay();
	M_ForceField = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(0), this);
	Mesh->SetMaterial(0, M_ForceField);
	M_Shield = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(1), this);
	Mesh->SetMaterial(1, M_Shield);
}

void ARangerShield::SetIsInUse(bool bInIsPlayerObject, const FVector& Location, const FRotator& Rotation)
{
	ShieldMaterialValueChanger = 0;
	PlaySound(ESound::RangerDeployShield);
	UpdateShieldMaterialValues(0.f, 0.7f);
	Super::SetIsInUse(bInIsPlayerObject, Location, Rotation);
	UKismetSystemLibrary::K2_SetTimer(this, "MoveShield", 0.05f, true);
}

void ARangerShield::DestroyThis()
{
	ShieldMaterialValueChanger = 0;
	PlaySound(ESound::RangerDissolveShield);
	UKismetSystemLibrary::K2_SetTimer(this, "DissolveShield", 0.05f, true);
}

void ARangerShield::UpdateShieldMaterialValues(const float InOpacity, const float InProgress) const
{
	M_ForceField->SetScalarParameterValue("Opacity", InOpacity);
	M_Shield->SetScalarParameterValue("Progress", InProgress);
}

void ARangerShield::LoadSounds()
{
	Sounds[ESound::RangerDeployShield]->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Sounds[ESound::RangerDissolveShield]->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void ARangerShield::PlaySound(const ESound InSoundName)
{
	Sounds[InSoundName]->Sound ? Sounds[InSoundName]->Play() : GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(Sounds[InSoundName], InSoundName);
}

void ARangerShield::MoveShield()
{
	if (++ShieldMaterialValueChanger >= 10) UKismetSystemLibrary::K2_ClearTimer(this, "MoveShield");
	SetActorLocation(GetActorLocation() + Proprietor->GetActorForwardVector() * 10.f);
	UpdateShieldMaterialValues(0.f + ShieldMaterialValueChanger * 0.05f, 0.7f - ShieldMaterialValueChanger * 0.03f);
}

void ARangerShield::DissolveShield()
{
	if (++ShieldMaterialValueChanger >= 10)
	{
		UKismetSystemLibrary::K2_ClearTimer(this, "DissolveShield");
		Super::DestroyThis();
	}
	UpdateShieldMaterialValues(0.5f - ShieldMaterialValueChanger * 0.05f, 0.4f + ShieldMaterialValueChanger * 0.03f);
}

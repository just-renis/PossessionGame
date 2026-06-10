#include "Throwable.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Data Assets/Base Data Assets/ThrowableData.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

void AThrowable::Throw()
{
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetPhysicsLinearVelocity(FVector::ZeroVector, false);
	Mesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	Mesh->AddImpulse((-GetActorRightVector() + GetActorForwardVector() * Data->GetDirectionMultiplier()).GetSafeNormal() * Data->GetImpulseForce(), NAME_None, true);
	SetupOnHitEvent();
}

void AThrowable::InitWithProprietor(const TObjectPtr<AEnemy> InProprietor)
{
	Super::InitWithProprietor(InProprietor);
	Proprietor = InProprietor;
	if (Data->IsVariationValuesExist()) P_SetVariationMaterial();
}

void AThrowable::Disable(const bool bHide)
{
	if (Data->GetNeedsOnHitEvent()) Mesh->OnComponentHit.RemoveAll(this);
	Super::Disable(bHide);
}

AThrowable::AThrowable()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;
}

void AThrowable::P_SetVariationMaterial()
{
	if (!MID_Variation)
	{
		MID_Variation = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(0), this);
		Mesh->SetMaterial(0, MID_Variation);
		if (!MID_Variation) return;
	}
	Data->SetupVariationMaterials(TArray{MID_Variation}, Proprietor->GetVariation());
}

float AThrowable::GetPlayerLevelAllGlobalDamageBonus() const
{
	return Data->GetDamage() * GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetPlayerLevelAllGlobalDamageBonus();
}

void AThrowable::OnImpact(const FHitResult& InHit)
{
	Mesh->OnComponentHit.RemoveAll(this);
	SetActorLocation(InHit.ImpactNormal, true);
	SetActorRotation(FRotator::ZeroRotator);
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AThrowable::PlaySound(const ESound InSoundName)
{
	Sounds[InSoundName]->Sound ? Sounds[InSoundName]->Play() :
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(Sounds[InSoundName], TFunction<bool()>(), InSoundName);
}

void AThrowable::SetupOnHitEvent() { if (Data->GetNeedsOnHitEvent() && !Mesh->OnComponentHit.IsBound()) Mesh->OnComponentHit.AddDynamic(this, &AThrowable::OnHit); }

#include "ShieldProjector.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/OverlapResult.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Data Assets/Base Data Assets/DestructibleData.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

AShieldProjector::AShieldProjector()
{
	ProjectorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectorMesh"));
	RootComponent = ProjectorMesh;
	Mesh->SetupAttachment(RootComponent);
	DeploySfx = CreateDefaultSubobject<UAudioComponent>(TEXT("DeploySfx"));
	DeploySfx->SetAutoActivate(false);
	DeploySfx->SetupAttachment(ProjectorMesh);
	DissolveSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("DissolveSfx"));
	DissolveSfx->SetAutoActivate(false);
	DissolveSfx->SetupAttachment(ProjectorMesh);
	Sounds.Add(ESound::TankShieldDeploy, DeploySfx);
	Sounds.Add(ESound::TankShieldDissolve, DissolveSfx);
}

void AShieldProjector::P_ShieldDestruction() const
{
	if (!Mesh) return;
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Proprietor);
	DrawDebugSphere(GetWorld(), Mesh->GetComponentLocation(), Mesh->Bounds.SphereRadius, 32, FColor::Green, false, 5.f);
	if (GetWorld()->OverlapMultiByObjectType(Overlaps, Mesh->GetComponentLocation(), FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(Mesh->Bounds.SphereRadius), Params))
	{
		for (const FOverlapResult& Result : Overlaps)
		{
			AEnemy* Enemy = Cast<AEnemy>(Result.GetActor());
			if (Enemy && Enemy->GetIsAlive()) Enemy->ApplyTankShieldActionSpeedDebuff();
		}
	}
}

void AShieldProjector::BeginPlay()
{
	Super::BeginPlay();
	ProjectorMesh->OnComponentHit.AddDynamic(this, &AShieldProjector::OnProjectorLanded);
}

void AShieldProjector::InitWithProprietor(const TObjectPtr<AEnemy> InProprietor)
{
	Super::InitWithProprietor(InProprietor);
	Proprietor = InProprietor;
	if (!ConnectionVfx) GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::TankShieldConnection,[this]() -> bool { return ProjectorMesh->GetVisibleFlag(); }, &ConnectionVfx, ProjectorMesh);
	else ConnectionVfx->Activate();
	ChangeProjectorCollisionState(true);
}

void AShieldProjector::DestroyThis()
{
	if (bIsPlayerObject && GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->IsSpecialUpgradeAvailable(EUpgrade::TankAbility1, EUpgradeAttribute::Special1)) P_ShieldDestruction();
	ChangeProjectorCollisionState(false);
	Super::DestroyThis();
}

void AShieldProjector::ChangeProjectorCollisionState(const bool bState) const
{
	ProjectorMesh->SetSimulatePhysics(bState);
	ProjectorMesh->SetCollisionEnabled(bState ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	ProjectorMesh->SetNotifyRigidBodyCollision(bState);
	if (!Mesh) return;
	Mesh->SetVisibility(!bState);
	Mesh->SetCollisionEnabled(bState ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryOnly);
}

void AShieldProjector::SetThisLifeSpan()
{
	const float EffectDurationIncrease = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::TankAbility1, EUpgradeAttribute::EffectDuration, bIsPlayerObject);
	LifeSpan = Data->GetLifeSpan() + Data->GetLifeSpan() * EffectDurationIncrease * 0.01f;
	Super::SetThisLifeSpan();
}

void AShieldProjector::SetUpgradesApplication()
{
	const TObjectPtr<URunSaveSlot> RunSaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot();
	Hp = Data->GetHp() + RunSaveSlot->GetUpgradeAttributeValues(EUpgrade::TankAbility1, EUpgradeAttribute::FlatHealth, bIsPlayerObject);
	const float ReductionIncrease = RunSaveSlot->GetUpgradeAttributeValues(EUpgrade::TankAbility1, EUpgradeAttribute::ReduceDamageTaken, bIsPlayerObject);
	DamageReduction = Data->GetDamageReduction() / (1.f + ReductionIncrease * 0.01f);
}

void AShieldProjector::Disable(const bool bHide)
{
	PlaySound(ESound::TankShieldDissolve);
	Super::Disable(bHide);
	if (ConnectionVfx) ConnectionVfx->DeactivateImmediate();
}

void AShieldProjector::PlaySound(const ESound InSoundName)
{
	Sounds[InSoundName]->Sound ? Sounds[InSoundName]->Play() : GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(Sounds[InSoundName], TFunction<bool()>(), InSoundName);
}

void AShieldProjector::OnProjectorLanded(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!ProjectorMesh) return;
	PlaySound(ESound::TankShieldDeploy);
	ChangeProjectorCollisionState(false);
}

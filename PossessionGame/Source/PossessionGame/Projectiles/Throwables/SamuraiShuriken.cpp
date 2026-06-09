#include "SamuraiShuriken.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void ASamuraiShuriken::InitWithProprietor(const TObjectPtr<AEnemy> InProprietor)
{
	Super::InitWithProprietor(InProprietor);
	if (!ItselfVfx) GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::PlayerSamuraiShuriken, [this]() -> bool { return Mesh->GetVisibleFlag(); }, &ItselfVfx, Mesh, NAME_None, Proprietor->GetVariation());
	else ItselfVfx->Activate();
}

ASamuraiShuriken::ASamuraiShuriken()
{
	ImpactSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("ImpactSfx"));
	ImpactSfx->SetAutoActivate(false);
	ImpactSfx->SetupAttachment(Mesh);
	Sounds.Add(ESound::SamuraiShurikenImpact, ImpactSfx);
}

void ASamuraiShuriken::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) { OnImpact(Hit); }

void ASamuraiShuriken::OnImpact(const FHitResult& InHit)
{
	Super::OnImpact(InHit);
	if (!ImpactVfx) GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::PlayerSamuraiShurikenImpact, [this]() -> bool { return Mesh->GetVisibleFlag(); }, &ImpactVfx, Mesh, NAME_None, Proprietor->GetVariation());
	else ImpactVfx->Activate();
	PlaySound(ESound::SamuraiShurikenImpact);
}

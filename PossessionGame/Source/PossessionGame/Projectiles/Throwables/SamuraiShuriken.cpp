#include "SamuraiShuriken.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void ASamuraiShuriken::InitWithProprietor(const TObjectPtr<AEnemy> InProprietor)
{
	Super::InitWithProprietor(InProprietor);
	Mesh->OnComponentBeginOverlap.AddDynamic(this, &ASamuraiShuriken::OnOverlapBegin);
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
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->SpawnVfx(EVfx::PlayerSamuraiShurikenImpact, nullptr, InHit.ImpactPoint, InHit.ImpactNormal.Rotation(), Proprietor->GetVariation());
	PlaySound(ESound::SamuraiShurikenImpact);
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::SamuraiShuriken, this);
}

void ASamuraiShuriken::Disable(const bool bHide)
{
	Mesh->OnComponentBeginOverlap.RemoveAll(this);
	Super::Disable(bHide);
}

void ASamuraiShuriken::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || (!OtherActor->ActorHasTag(FName("Enemy")) && !OtherActor->ActorHasTag(FName("Destructible")))) return;
	if (!ImpactVfx) GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::PlayerSamuraiShurikenImpact, [this]() -> bool { return Mesh->GetVisibleFlag(); }, &ImpactVfx, Mesh, NAME_None, Proprietor->GetVariation());
	else ImpactVfx->Activate();
	UGameplayStatics::ApplyDamage(OtherActor, 25.f, GetInstigatorController(), this, UDamageType::StaticClass());
}

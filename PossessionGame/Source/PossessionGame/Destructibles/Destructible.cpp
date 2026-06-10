#include "Destructible.h"
#include "Engine/DamageEvents.h"
#include "PossessionGame/Data Assets/Base Data Assets/DestructibleData.h"
#include "PossessionGame/Projectiles/Projectile.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"

ADestructible::ADestructible()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	RootComponent = Mesh;
}

void ADestructible::HandleDeactivation()
{
	bIsDeactivated = true;
	if (Mesh)
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Mesh->SetSimulatePhysics(true);
	}
	SetThisLifeSpan();
}

void ADestructible::InitWithProprietor(const TObjectPtr<AEnemy> InProprietor)
{
	Super::InitWithProprietor(InProprietor);
	Proprietor = InProprietor;
	Hp = Data->GetHp();
	bIsAffectedByPhysics = Data->GetIsAffectedByPhysics();
	bIsPlayerObject = Proprietor->GetIsHijacked();
	Mesh->SetCollisionProfileName(Data->GetCollisionProfileName(bIsPlayerObject));
	if (Data->GetIsLifeSpanImmediatelyAdded()) SetThisLifeSpan();
	SetUpgradesApplication();
	if (Data->IsVariationValuesExist()) P_SetVariationMaterial();
}

void ADestructible::ReceiveDamage(const float InDamage)
{
	Hp -= InDamage;
	if (!bIsPlayerObject) (void)GetGameInstance()->GetSubsystem<UDelegateManager>()->ShowDamageNumber.ExecuteIfBound(FLinearColor::White, InDamage, GetActorLocation(), GetActorForwardVector());
	if (Hp > 0) return;
	if (bIsAffectedByPhysics) HandleDeactivation();
	else DestroyThis();
}

void ADestructible::DestroyThis()
{
	GetWorldTimerManager().ClearTimer(DestroyHandle);
	if (Mesh) Mesh->SetSimulatePhysics(false);
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(Data->GetObjectName(), this);
}

void ADestructible::P_SetVariationMaterial()
{
	if (!MID_Variation)
	{
		MID_Variation = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this);
		GetMesh()->SetMaterial(0, MID_Variation);
		if (!MID_Variation) return;
	}
	Data->SetupVariationMaterials(TArray{ MID_Variation }, Proprietor->GetVariation());
}

float ADestructible::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser) * DamageReduction;
	if (!bIsPlayerObject) (void)GetGameInstance()->GetSubsystem<UDelegateManager>()->ShowDamageNumber.ExecuteIfBound(FLinearColor::White, Damage, GetActorLocation(), GetActorForwardVector());
	if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		if (const TObjectPtr<AProjectile> ProjectileCauser = Cast<AProjectile>(DamageCauser))
		{
			const bool bIsPlayerProjectile = ProjectileCauser->GetIsPlayerProjectile();
			if ((bIsPlayerProjectile && !bIsPlayerObject) || (!bIsPlayerProjectile && bIsPlayerObject)) SubtractHp(Damage);
		}
	}
	else SubtractHp(Damage);
	if (Hp > 0) return Damage;
	if (bIsAffectedByPhysics)
	{
		HandleDeactivation();
		if (!DamageEvent.IsOfType(FRadialDamageEvent::ClassID)) return Damage;
		const FRadialDamageEvent* RadialDamageEvent = static_cast<const FRadialDamageEvent*>(&DamageEvent);
		Mesh->AddRadialImpulse(RadialDamageEvent->Origin, RadialDamageEvent->Params.OuterRadius, 20000.f, RIF_Linear);
	}
	else DestroyThis();
	return Damage;
}

void ADestructible::BeginPlay()
{
	Super::BeginPlay();
	Tags.Add("Destructible");
}

void ADestructible::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(DestroyHandle);
	Super::EndPlay(EndPlayReason);
}

void ADestructible::SetThisLifeSpan()
{
	GetWorldTimerManager().SetTimer(DestroyHandle, this, &ADestructible::DestroyThis, LifeSpan, false);
}

void ADestructible::SubtractHp(const float InDamage) { Hp -= InDamage; }

void ADestructible::SetUpgradesApplication()
{
	DamageReduction = Data->GetDamageReduction();
	LifeSpan = Data->GetLifeSpan();
}

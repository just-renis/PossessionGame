#include "Dummy.h"
#include "PoolObject.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"

ADummy::ADummy()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	RootComponent = Mesh;
}

void ADummy::BeginPlay()
{
	Super::BeginPlay();
	Tags.Add("Destructible");
}

float ADummy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("DAMAGEAMOUNT: %f"), DamageAmount);
	GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::DamageNumber, GetActorLocation() + FVector(0.f, 0.f, 60.f) + GetActorForwardVector() * 50.f, FRotator::ZeroRotator)->Init(DamageAmount);
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

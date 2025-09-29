#include "Dummy.h"

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
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

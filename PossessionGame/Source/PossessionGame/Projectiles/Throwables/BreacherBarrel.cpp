#include "BreacherBarrel.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"

void ABreacherBarrel::Explode()
{
	UE_LOG(LogTemp, Error, TEXT("EXPLODE"));
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::BreacherBarrel, this);
}

void ABreacherBarrel::Disable(const bool bHide)
{
	Mesh->SetSimulatePhysics(false);
	Super::Disable(bHide);
}

void ABreacherBarrel::BeginPlay()
{
	Super::BeginPlay();
	Tags.Add(FName("BreacherBarrel"));
}

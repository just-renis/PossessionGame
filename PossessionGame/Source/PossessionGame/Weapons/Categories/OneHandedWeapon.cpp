#include "OneHandedWeapon.h"

AOneHandedWeapon::AOneHandedWeapon()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("root"));
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("weaponMesh"));
	Mesh->SetupAttachment(RootComponent);
}

void AOneHandedWeapon::BeginPlay()
{
	Super::BeginPlay();
	Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh->SetComponentTickEnabled(false);
}

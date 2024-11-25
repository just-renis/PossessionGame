#include "OneHandedWeapon.h"

AOneHandedWeapon::AOneHandedWeapon()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("root"));
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("weaponMesh"));
	Mesh->SetupAttachment(RootComponent);
}

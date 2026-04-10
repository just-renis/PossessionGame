#include "WireWithLight.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

AWireWithLight::AWireWithLight()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

void AWireWithLight::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnUsbInsertionForWires.AddUObject(this, &AWireWithLight::ChangeColor);
}

void AWireWithLight::ChangeColor(const int32 InId) const
{
	if (Id != InId) return;
	if (const TObjectPtr<UMaterialInstanceDynamic> MID_Mesh = Mesh->CreateDynamicMaterialInstance(0)) MID_Mesh->SetScalarParameterValue(FName("Hue"),0.66f);
}

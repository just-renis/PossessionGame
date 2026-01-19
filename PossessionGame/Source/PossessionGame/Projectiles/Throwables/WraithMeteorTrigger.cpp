#include "WraithMeteorTrigger.h"

void AWraithMeteorTrigger::BeginPlay()
{
	Super::BeginPlay();
	if (!Mesh->GetMaterial(0)) return;
	MI_MeshBaseMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(0);
	MI_MeshBaseMaterial->SetVectorParameterValue("Color", FLinearColor(0.571f, 0.f, 0.f));
}

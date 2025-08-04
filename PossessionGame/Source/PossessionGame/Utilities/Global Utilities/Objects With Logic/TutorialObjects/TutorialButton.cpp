#include "TutorialButton.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"

ATutorialButton::ATutorialButton()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	MeshFrame = CreateDefaultSubobject<UStaticMeshComponent>("MeshFrame");
	Collision = CreateDefaultSubobject<USphereComponent>("Collision");
	Collision->SetCollisionProfileName("TriggerPlayer");
	RootComponent = MeshFrame;
	Mesh->SetupAttachment(RootComponent);
	Collision->SetupAttachment(RootComponent);
}

void ATutorialButton::BeginPlay()
{
	Super::BeginPlay();
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ATutorialButton::OverlapOccured);
}

void ATutorialButton::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(ProcessAffectedObjectHandle);
	Super::EndPlay(EndPlayReason);
}

void ATutorialButton::ButtonClicked()
{
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetActorLocation(GetActorLocation() - FVector(0.f, 0.f, 40.f));
	SetupAffectedObjectLogic();
}

void ATutorialButton::SetupAffectedObjectLogic()
{
	
//	if (!AffectedObject) return;
	//MID_AffectedObject = AffectedObject->CreateAndSetMaterialInstanceDynamic(AffectedObjectMaterialIndex);
	GetWorldTimerManager().SetTimer(ProcessAffectedObjectHandle, this, &ATutorialButton::ProcessAffectedObjectLogic, 0.025f, true);
}

void ATutorialButton::ProcessAffectedObjectLogic()
{
	if (!MID_AffectedObject) return;
	PatternOpacityIterationIndex++;
	MID_AffectedObject->SetScalarParameterValue("PatternMaxOpacity", 0.25f - 0.0125f * PatternOpacityIterationIndex); 
	MID_AffectedObject->SetScalarParameterValue("PatternOpacityDivisor", 2.f - 0.1f * PatternOpacityIterationIndex);
	if (PatternOpacityIterationIndex >= 20) GetWorldTimerManager().ClearTimer(ProcessAffectedObjectHandle);
}

void ATutorialButton::OverlapOccured(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player")) return;
	if (const TObjectPtr<AEnemy> Player = Cast<AEnemy>(OtherActor)) if (Player->GetIsStomping()) ButtonClicked();
}

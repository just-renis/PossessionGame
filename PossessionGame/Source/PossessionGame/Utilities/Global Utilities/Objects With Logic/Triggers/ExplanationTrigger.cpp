#include "ExplanationTrigger.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/TutorialManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

AExplanationTrigger::AExplanationTrigger()
{
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	RootComponent = Collision;
}

void AExplanationTrigger::BeginPlay()
{
	Super::BeginPlay();
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AExplanationTrigger::Initiate);
}

void AExplanationTrigger::Initiate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player")) return;
	(void)GetWorld()->GetSubsystem<UTutorialManager>()->OnFpsExplanationTriggered.ExecuteIfBound();
	(void)GetWorld()->GetSubsystem<UWidgetManager>()->GetOnShowExplanationDelegate().ExecuteIfBound(Explanation, Cast<UPossessionGameInstance>(GetGameInstance()));
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Destroy();
}

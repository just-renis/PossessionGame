#include "Terminal.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"

ATerminal::ATerminal()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("terminalMesh"));
	InteractableWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("interactableWidgetComponent"));
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("collision"));
	RootComponent = Mesh;
	Collision->SetupAttachment(RootComponent);
	InteractableWidget->SetupAttachment(RootComponent);
}

void ATerminal::BeginPlay()
{
	Super::BeginPlay();
	if (InteractableWidget) InteractableWidget->SetVisibility(false);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ATerminal::EnableInteractionWidget);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ATerminal::DisableInteractionWidget);
}

void ATerminal::Interact()
{
	
}

void ATerminal::EnableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(OtherActor))
	{
		if (Enemy->GetIsPossessed()) InteractableWidget->SetVisibility(true);
	}
}

void ATerminal::DisableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (const TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(OtherActor))
	{
		if (Enemy->GetIsPossessed()) InteractableWidget->SetVisibility(false);
	}
}

#include "DamageNumber.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/UI/SecondaryWidgets/DamageNumberUI.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"

ADamageNumber::ADamageNumber()
{
	DamageNumberWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("damageNumberWidget");
	RootComponent = DamageNumberWidgetComponent;
}

void ADamageNumber::BeginPlay()
{
	Super::BeginPlay();
	DamageNumber = Cast<UDamageNumberUI>(DamageNumberWidgetComponent->GetWidget());
}

void ADamageNumber::Init(const float InData)
{
	Super::Init(InData);
	DamageNumber->SetDamageAmount(InData);
	UE_LOG(LogTemp, Warning, TEXT("ANIMATION LENGTH: %f"), DamageNumber->GetAnimationLength());
	UKismetSystemLibrary::K2_SetTimer(this, "ReturnToPool", DamageNumber->GetAnimationLength(), false);
}

void ADamageNumber::ReturnToPool() { GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::DamageNumber, this); }

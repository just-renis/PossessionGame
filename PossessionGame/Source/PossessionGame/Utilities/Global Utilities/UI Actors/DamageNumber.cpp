#include "DamageNumber.h"
#include "Components/WidgetComponent.h"
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

void ADamageNumber::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(ReturnToPoolHandle);
	Super::EndPlay(EndPlayReason);
}

void ADamageNumber::Init(const float InData, const bool bHide)
{
	Super::Init(InData, false);
	DamageNumber->SetDamageAmount(InData);
	GetWorldTimerManager().SetTimer(ReturnToPoolHandle, this, &ADamageNumber::ReturnToPool, DamageNumber->GetAnimationLength(), false);
}

void ADamageNumber::Disable(const bool bHide) { Super::Disable(false); }

void ADamageNumber::ReturnToPool() { GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::DamageNumber, this); }

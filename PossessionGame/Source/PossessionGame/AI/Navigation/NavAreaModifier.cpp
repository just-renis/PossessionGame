#include "NavAreaModifier.h"
#include "NavModifierComponent.h"
#include "Components/BoxComponent.h"

ANavAreaModifier::ANavAreaModifier()
{
	NavModifierComponent = CreateDefaultSubobject<UNavModifierComponent>(TEXT("NavModifierComponent"));
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	RootComponent = Collision;
}

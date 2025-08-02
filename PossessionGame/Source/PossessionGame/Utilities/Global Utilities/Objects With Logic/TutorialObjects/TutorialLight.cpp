#include "TutorialLight.h"
#include "Components/LightComponent.h"
#include "Components/RectLightComponent.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

ATutorialLight::ATutorialLight()
{
	RectLight = CreateDefaultSubobject<URectLightComponent>(TEXT("RectLight"));
	RootComponent = RectLight;
}

void ATutorialLight::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnAlertStart.AddUObject(this, &ATutorialLight::SetupBlinking);
}

void ATutorialLight::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnAlertStart.RemoveAll(this);
	GetWorldTimerManager().ClearTimer(BlinkingHandle);
	Super::EndPlay(EndPlayReason);
}

void ATutorialLight::SetupBlinking()
{
	RectLight->SetLightFColor(FColor(101.f, 19.f, 0.f));
	GetWorldTimerManager().SetTimer(BlinkingHandle, this, &ATutorialLight::Blinking, 0.5f, true);
}

void ATutorialLight::Blinking()
{
	bIsOn = !bIsOn;
	RectLight->SetIntensity(bIsOn ? 5.f : 1.f);
}

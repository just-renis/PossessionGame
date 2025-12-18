#include "JumpPad.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

AJumpPad::AJumpPad()
{
	PrimaryActorTick.bCanEverTick = false;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
	RootComponent = Mesh;
	Collision = CreateDefaultSubobject<UBoxComponent>(FName("Collision"));
	Collision->SetupAttachment(Mesh);
	Collision->SetCollisionProfileName(FName("TriggerPlayer"));
	Collision->SetGenerateOverlapEvents(true);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AJumpPad::OnOverlapBegin);
	JumpPadActivateSound = CreateDefaultSubobject<UAudioComponent>(FName("JumpPadActivateSound"));
	JumpPadActivateSound->SetAutoActivate(false);
	JumpPadActivateSound->SetupAttachment(Mesh);
}

void AJumpPad::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player")) return;
	Cast<ACharacter>(OtherActor)->LaunchCharacter(LaunchVelocity, false, true);
	JumpPadActivateSound->Sound ? JumpPadActivateSound->Play() : GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(JumpPadActivateSound, nullptr, ESound::JumpPadActivate);
}

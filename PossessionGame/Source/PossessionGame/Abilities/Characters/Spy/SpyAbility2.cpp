#include "SpyAbility2.h"
#include "Camera/CameraComponent.h"
#include "PossessionGame/AI/Enemies/Characters/Spy.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Enums/EMoveMode.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void USpyAbility2::AI_Execute()
{
	if (!CanExecute() || !DelegateManager->RequestBubbleLocation.IsBound()) return;
	const FVector BubbleLocation = DelegateManager->RequestBubbleLocation.Execute();
	if (!BubbleLocation.ContainsNaN())
	{
		Proprietor->GetAI()->SetTargetLocation(BubbleLocation);
		Proprietor->GetAI()->SetIsUsingEQS(false);
		Proprietor->AI_SetMoveMode(EMoveMode::Run);
		Proprietor->SetIsAllowedToShoot(false);
	}
	else return;
	Super::AI_Execute();
}

void USpyAbility2::P_Execute() { CastLineTrace(); }

bool USpyAbility2::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void USpyAbility2::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<ASpy>(InAbilityProprietor); }

void USpyAbility2::CastLineTrace()
{
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	const FVector StartLocation = Proprietor->GetCamera()->GetComponentLocation();
	const FVector EndLocation = StartLocation + Proprietor->GetCamera()->GetForwardVector() * 15000.f;
	QueryParams.AddIgnoredActor(Proprietor);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, Proprietor->GetXrayState() ? ECC_GameTraceChannel3 : ECC_GameTraceChannel5, QueryParams)) TryHitEnemy(HitResult);
}

void USpyAbility2::TryHitEnemy(const FHitResult& HitResult)
{
	if (!HitResult.GetActor()->ActorHasTag("Enemy")) return;
	Target = Cast<AEnemy>(HitResult.GetActor());
	if (!Target || Target->IsBoss()) return;
	TargetLocation = Target->GetActorLocation();
	if (Target->GetIsAlive()) InitiateProcess();
}

void USpyAbility2::InitiateProcess()
{
	if (Proprietor->GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestSpecialUpgradeAvailability.Execute(EUpgrade::SpyAbility2, EUpgradeAttribute::Special1))
	{
		if (FMath::RandRange(1, 10) <= 3)
		{
			Super::P_Execute();
			return;
		}
	}
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontageWithHold(EHandsAnimState::TeleportTravel, -1.f);
	Proprietor->SetIsActionInMotion(true);
	Proprietor->StartTeleportProcess();
	Proprietor->SetInputState(true);
	Proprietor->GetWorldTimerManager().SetTimer(TransferHandle, this, &USpyAbility2::ProcessTransfer, 0.02f, true);
	Super::P_Execute();
}

void USpyAbility2::ProcessTransfer()
{
	const FVector CurrentCameraLocation = Proprietor->GetCamera()->GetComponentLocation();
	const FVector Direction = (TargetLocation - CurrentCameraLocation).GetSafeNormal();
	const FVector NewLocation = CurrentCameraLocation + Direction * TransferSpeed;
	Proprietor->GetCamera()->SetWorldLocation(NewLocation);
	const float Distance = FVector::Dist(NewLocation, TargetLocation);
	if (Distance <= TransferSpeed) FinishTransfer();
}

void USpyAbility2::FinishTransfer()
{
	Proprietor->FinishTeleportProcess();
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::TeleportImpact);
	Proprietor->SetInputState(false);
	Proprietor->GetWorldTimerManager().ClearTimer(TransferHandle);
	if (Target && Target->GetIsAlive()) Target->InitiateDeath();
	Proprietor->SetActorLocation(TargetLocation);
}

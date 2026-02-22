#include "EnemyCommunicator.h"
#include "DelegateManager.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/TeleportBubble.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameGameModeBase.h"

FVector UEnemyCommunicator::GetPointLocation(ECharacterType InCharacterType, FVector InLocation)
{
	if (PointsAroundTarget.IsEmpty()) return GetTargetLocation();
	TArray<int32> AvailableKeys;
	float MinDistance = FLT_MAX;
	int32 KeyIndex = 0;
	for (const TPair<int32, FEQSPointData>& Entry : PointsAroundTarget)
	{
		if (!Entry.Value.bIsOccupied && Entry.Value.ViableCharacters.Contains(InCharacterType))
		{
			const float Distance = (InLocation - Entry.Value.Location).SizeSquared();
			if (MinDistance > Distance)
			{
				MinDistance = Distance;
				KeyIndex = Entry.Key;
			}
			AvailableKeys.Add(Entry.Key);
		}
	}
	if (AvailableKeys.IsEmpty()) return GetTargetLocation();
	PointsAroundTarget[KeyIndex].bIsOccupied = true;
	return PointsAroundTarget[KeyIndex].Location;
}

FVector UEnemyCommunicator::GetTargetLocation() const { return Target ? Target->GetActorLocation() : FVector(NAN, NAN, NAN); }

// DO NOT USE, INCORRECT AND CRASH INDULGING, REWORK IF THIS FUNCTION WILL EVER BE NEEDED.
FVector UEnemyCommunicator::GetFarthestEnemyLocation(const FVector& InFromLocation)
{
	float Distance = 0.f;
	int32 Index = 0;
	for (int32 i = 0; i < Enemies.Num(); i++)
	{
		const float CalcDistance = FVector::DistSquared(InFromLocation, Enemies[i]->GetActorLocation());
		if (CalcDistance > Distance)
		{
			Distance = CalcDistance;
			Index = i;
		}
	}
	return Enemies[Index]->GetActorLocation();
}

TObjectPtr<AActor> UEnemyCommunicator::GetClosestEnemy(const FVector& InFromLocation, const TObjectPtr<AActor>& InIgnoreEnemy)
{
	if (!GetIsEnemiesAvailable()) return nullptr;
	float Distance = FLT_MAX;
	int32 Index = -1;
	for (int32 i = 0; i < Enemies.Num(); i++)
	{
		if (InIgnoreEnemy == Enemies[i]) continue;
		const float CalcDistance = FVector::DistSquared(InFromLocation, Enemies[i]->GetActorLocation());
		if (CalcDistance < Distance)
		{
			Distance = CalcDistance;
			Index = i;
		}
	}
	if (Index == -1) return nullptr;
	return Enemies[Index];
}

void UEnemyCommunicator::Init(TObjectPtr<UDelegateManager> InDelegateManager, TObjectPtr<UEnvQuery> InTargetEQS)
{
	InitDelegates(InDelegateManager);
	TargetEQS = InTargetEQS;
	if (WITH_EDITOR) GameModeBase = Cast<APossessionGameGameModeBase>(GetWorld()->GetAuthGameMode());
}

void UEnemyCommunicator::SetTarget(const TObjectPtr<AEnemy>& InTarget)
{
	Target = InTarget;
	DelegateManager->OnTargetChange.Broadcast(InTarget, bWasTargetSet);
	if (!bWasTargetSet) bWasTargetSet = true;
	GetWorld()->GetTimerManager().SetTimer(EQSPointsGenerationHandle, this, &UEnemyCommunicator::GeneratePointsAroundTarget, 1.f, true);
}

void UEnemyCommunicator::AddEnemy(const TObjectPtr<AActor>& InEnemy) { Enemies.Add(InEnemy); }

void UEnemyCommunicator::RemoveEnemy(const TObjectPtr<AActor>& InEnemy) { Enemies.Remove(InEnemy); }

void UEnemyCommunicator::AddPoint(const int32 InKey, const FEQSPointData& InData) { PointsAroundTarget.Add(InKey, InData); }

void UEnemyCommunicator::RemovePoint(const int32 InKey) { PointsAroundTarget.Remove(InKey); }

void UEnemyCommunicator::InitDelegates(const TObjectPtr<UDelegateManager>& InDelegateManager)
{
	DelegateManager = InDelegateManager;
	DelegateManager->OnConcealAbilityChange.AddUObject(this, &UEnemyCommunicator::SetTarget);
	DelegateManager->OnToBubbleTeleport.BindUObject(this, &UEnemyCommunicator::TeleportToBubble);
}

void UEnemyCommunicator::TargetQueryResult(TSharedPtr<FEnvQueryResult> Result)
{
	OnEQSFinish.Broadcast();
	#if WITH_EDITOR
		if (!Result->IsSuccessful() || !GameModeBase || !GameModeBase->GetIsDebugModeEnabled()) return;
		for (TPair<int32, FEQSPointData> PointData : PointsAroundTarget)
		{
			const FEQSPointData& Data = PointData.Value;
			switch (TSet<ECharacterType>::TConstIterator It = Data.ViableCharacters.CreateConstIterator(); *It)
			{
				case ECharacterType::Minion:
					DrawDebugSphere(GetWorld(), Data.Location, 20.f, 16, FColor::Blue, false, 1.0f);
					break;
				case ECharacterType::Ranger:
					DrawDebugSphere(GetWorld(), Data.Location, 20.f, 16, FColor::Red, false, 1.0f);
					break;
				case ECharacterType::Trickster:
					DrawDebugSphere(GetWorld(), Data.Location, 20.f, 16, FColor::Green, false, 1.0f);
					break;
				default: ;
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("EQS Query Result: Failure - Reason: %s"), *UEnum::GetValueAsString(Result->GetRawStatus()));
	#endif
}

void UEnemyCommunicator::GeneratePointsAroundTarget()
{
	PointsAroundTarget.Empty();
	FEnvQueryRequest QueryRequest(TargetEQS, Target);
	QueryRequest.Execute(EEnvQueryRunMode::AllMatching, this, &UEnemyCommunicator::TargetQueryResult);
}

void UEnemyCommunicator::Deinitialize()
{
	if (GetWorld()) GetWorld()->GetTimerManager().ClearTimer(EQSPointsGenerationHandle);
	Super::Deinitialize();
}

void UEnemyCommunicator::TeleportToBubble(const TObjectPtr<AActor>& InActor)
{
	if (!InActor) return;
	TArray<TObjectPtr<ATeleportBubble>> ValidBubbles;
	const EGameMode CurrentGameMode = DelegateManager->RequestCurrentGameMode.Execute();
	UE_LOG(LogTemp, Error, TEXT("CURRENT GAME MODE: %s"), *UEnum::GetValueAsString(CurrentGameMode));
	for (const TObjectPtr<ATeleportBubble>& Bubble : TeleportBubbles)
	{
		if (Bubble && Bubble->IsCorrectGameMode(CurrentGameMode)) ValidBubbles.Add(Bubble);
	}
	if (ValidBubbles.IsEmpty()) return;
	const int32 Index = FMath::RandRange(0, ValidBubbles.Num() - 1);
	ValidBubbles[Index]->Teleport(InActor);
}

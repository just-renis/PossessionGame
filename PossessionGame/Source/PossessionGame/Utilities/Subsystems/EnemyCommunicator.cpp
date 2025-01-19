#include "EnemyCommunicator.h"
#include "DelegateManager.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Enums/ECharacterType.h"

FVector UEnemyCommunicator::GetPointLocation(ECharacterType InCharacterType, FVector InLocation)
{
	if (PointsAroundTarget.IsEmpty()) return FVector(NAN, NAN, NAN);
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
	if (AvailableKeys.IsEmpty()) return FVector(NAN, NAN, NAN);
	PointsAroundTarget[KeyIndex].bIsOccupied = true;
	return PointsAroundTarget[KeyIndex].Location;
}

FVector UEnemyCommunicator::GetFarthestEnemyLocation(FVector FromLocation)
{
	float Distance = 0.f;
	int32 Index = 0;
	for (int32 i = 0; i < Enemies.Num(); i++)
	{
		float CalcDistance = FVector::Distance(FromLocation, Enemies[i]->GetActorLocation());
		if (CalcDistance > Distance)
		{
			Distance = CalcDistance;
			Index = i;
		}
	}
	return Enemies[Index]->GetActorLocation();
}

void UEnemyCommunicator::Init(TObjectPtr<UDelegateManager> InDelegateManager, TObjectPtr<UEnvQuery> InTargetEQS)
{
	InitDelegates(InDelegateManager);
	TargetEQS = InTargetEQS;
}

void UEnemyCommunicator::SetTarget(TObjectPtr<AActor> InTarget)
{
	Target = InTarget;
	DelegateManager->OnTargetChange.Broadcast(InTarget, bWasTargetSet);
	if (!bWasTargetSet) bWasTargetSet = true;
	UKismetSystemLibrary::K2_SetTimer(this, "GeneratePointsAroundTarget", 1.f, true);
}

void UEnemyCommunicator::AddEnemy(TObjectPtr<AActor> InEnemy) { Enemies.Add(InEnemy); }

void UEnemyCommunicator::RemoveEnemy(TObjectPtr<AActor> InEnemy) { Enemies.Remove(InEnemy); }

void UEnemyCommunicator::AddPoint(int32 Key, const FEQSPointData& Data) { PointsAroundTarget.Add(Key, Data); }

void UEnemyCommunicator::RemovePoint(int32 Key) { PointsAroundTarget.Remove(Key); }

void UEnemyCommunicator::InitDelegates(TObjectPtr<UDelegateManager> InDelegateManager)
{
	DelegateManager = InDelegateManager;
	DelegateManager->OnEnemiesAmountDecrease.AddUObject(this, &UEnemyCommunicator::SetEnemiesAmount);
	DelegateManager->OnConcealAbilityChange.AddUObject(this, &UEnemyCommunicator::SetTarget);
}

void UEnemyCommunicator::TargetQueryResult(TSharedPtr<FEnvQueryResult> Result)
{
	if (Result->IsSuccessful())
	{
		OnEQSFinish.Broadcast();
		for (TPair<int32, FEQSPointData> PointData : PointsAroundTarget)
		{
			const FEQSPointData& Data = PointData.Value;
			switch (TSet<ECharacterType>::TConstIterator It = Data.ViableCharacters.CreateConstIterator(); *It)
			{
				case ECharacterType::Minion:
				//	DrawDebugSphere(GetWorld(), Data.Location, 20.f, 16, FColor::Blue, false, 1.0f);
					break;
				case ECharacterType::Ranger: break;
			//		DrawDebugSphere(GetWorld(), Data.Location, 20.f, 16, FColor::Red, false, 1.0f);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EQS Query Result: Failure - Reason: %s"), *UEnum::GetValueAsString(Result->GetRawStatus()));
	}
}

void UEnemyCommunicator::SetEnemiesAmount(int32 Amount) { EnemiesAmount = Amount; }

void UEnemyCommunicator::GeneratePointsAroundTarget()
{
	PointsAroundTarget.Empty();
	FEnvQueryRequest QueryRequest(TargetEQS, Target);
	QueryRequest.Execute(EEnvQueryRunMode::AllMatching, this, &UEnemyCommunicator::TargetQueryResult);
}

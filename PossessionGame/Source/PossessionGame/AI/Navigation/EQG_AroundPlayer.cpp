#include "EQG_AroundPlayer.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Utilities/Data Utilities/FEQSPointData.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"

UEQG_AroundPlayer::UEQG_AroundPlayer() { ItemType = UEnvQueryItemType_Point::StaticClass(); }

void UEQG_AroundPlayer::GenerateItems(FEnvQueryInstance& QueryInstance) const
{
	const TObjectPtr<UEnemyCommunicator> EnemyCommunicator = GetWorld()->GetSubsystem<UEnemyCommunicator>();
	const int32 NumRings = 2;
	const int32 NumPointsInRing = 12;
	const TObjectPtr<AActor> QueryActor = Cast<AActor>(QueryInstance.Owner.Get());
	QueryInstance.ReserveItemData(NumRings * NumPointsInRing);
	for (int32 RingIndex = 0, i = 0; RingIndex < NumRings; RingIndex++)
	{
		const float CurrentRadius = 400.f + RingIndex * 100.f;
		for (int32 PointIndex = 0; PointIndex < NumPointsInRing; PointIndex++, i++)
		{
			const float Angle = FMath::DegreesToRadians(360.f / NumPointsInRing * PointIndex);
			FVector PointOffset = FVector(FMath::Cos(Angle) * CurrentRadius, FMath::Sin(Angle) * CurrentRadius, 0.f);
			FVector PointLocation = QueryActor->GetActorLocation() + PointOffset;
			QueryInstance.AddItemData<UEnvQueryItemType_Point>(PointLocation);
			EnemyCommunicator->AddPoint(i, FEQSPointData(PointLocation, RingIndex == 0 ? TSet{ECharacterType::Ranger} : TSet{ECharacterType::Minion}));
		}
	}
}

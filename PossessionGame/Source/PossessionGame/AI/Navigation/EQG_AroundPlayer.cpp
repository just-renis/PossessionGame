#include "EQG_AroundPlayer.h"
#include "NavigationSystem.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "PossessionGame/Utilities/Data Utilities/FEQSPointData.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"

UEQG_AroundPlayer::UEQG_AroundPlayer() { ItemType = UEnvQueryItemType_Point::StaticClass(); }

void UEQG_AroundPlayer::GenerateItems(FEnvQueryInstance& QueryInstance) const
{
	const TObjectPtr<UEnemyCommunicator> EnemyCommunicator = GetWorld()->GetSubsystem<UEnemyCommunicator>();
	constexpr int32 NumRings = 3;
	constexpr int32 NumPointsInRing = 12;
	const TObjectPtr<AActor> QueryActor = Cast<AActor>(QueryInstance.Owner.Get());
	if (!QueryActor) return;
	const TObjectPtr<UNavigationSystemV1> NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys) return;
	QueryInstance.ReserveItemData(NumRings * NumPointsInRing);
	for (int32 RingIndex = 0, i = 0; RingIndex < NumRings; RingIndex++)
	{
		const float CurrentRadius = DistanceRanges[RingIndex] * 100.f;
		if (const FDistanceRangeCharacters* Characters = DistanceRangeCharacters.Find(RingIndex))
		{
			for (int32 PointIndex = 0; PointIndex < NumPointsInRing; PointIndex++, i++)
			{
				const float Angle = FMath::DegreesToRadians(360.f / NumPointsInRing * PointIndex);
				FVector PointOffset = FVector(FMath::Cos(Angle) * CurrentRadius, FMath::Sin(Angle) * CurrentRadius, 0.f);
				FVector PointLocation = QueryActor->GetActorLocation() + PointOffset;
				FNavLocation NavPoint;
				if (NavSys->ProjectPointToNavigation(PointLocation, NavPoint, FVector(50.f, 50.f, 1000.f)))
				{
					FVector FixedLocation = NavPoint.Location + FVector(0.f, 0.f, 50.f);
					QueryInstance.AddItemData<UEnvQueryItemType_Point>(FixedLocation);
					EnemyCommunicator->AddPoint(i, FEQSPointData(FixedLocation, Characters->Values));
				}
				else i--;
			}
		}
	}
}

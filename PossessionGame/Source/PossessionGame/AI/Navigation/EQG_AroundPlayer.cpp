#include "EQG_AroundPlayer.h"
#include "NavigationSystem.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Utilities/Data Utilities/FEQSPointData.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"

UEQG_AroundPlayer::UEQG_AroundPlayer() { ItemType = UEnvQueryItemType_Point::StaticClass(); }

void UEQG_AroundPlayer::GenerateItems(FEnvQueryInstance& QueryInstance) const
{
	const TObjectPtr<UEnemyCommunicator> EnemyCommunicator = GetWorld()->GetSubsystem<UEnemyCommunicator>();
	constexpr int32 NumRings = 2;
	constexpr int32 NumPointsInRing = 12;
	const TObjectPtr<AActor> QueryActor = Cast<AActor>(QueryInstance.Owner.Get());
	const TObjectPtr<UNavigationSystemV1> NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	QueryInstance.ReserveItemData(NumRings * NumPointsInRing);
	for (int32 RingIndex = 0, i = 0; RingIndex < NumRings; RingIndex++)
	{
		const float CurrentRadius = DefaultMetresDistance * 100.f + RingIndex * RangerMetresDistance * 100.f;
		for (int32 PointIndex = 0; PointIndex < NumPointsInRing; PointIndex++, i++)
		{
			const float Angle = FMath::DegreesToRadians(360.f / NumPointsInRing * PointIndex);
			FVector PointOffset = FVector(FMath::Cos(Angle) * CurrentRadius, FMath::Sin(Angle) * CurrentRadius, 0.f);
			FVector PointLocation = QueryActor->GetActorLocation() + PointOffset;
			FNavLocation NavPoint;
			if (NavSys->ProjectPointToNavigation(PointLocation, NavPoint))
			{
				FVector FixedLocation = NavPoint.Location;
				FixedLocation.Z = PointLocation.Z;
				QueryInstance.AddItemData<UEnvQueryItemType_Point>(FixedLocation);
				EnemyCommunicator->AddPoint(i, FEQSPointData(FixedLocation, RingIndex == 0 ? TSet{ECharacterType::Minion} : TSet{ECharacterType::Ranger}));

			}
		}
	}
}

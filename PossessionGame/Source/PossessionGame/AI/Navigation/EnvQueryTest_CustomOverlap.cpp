#include "EnvQueryTest_CustomOverlap.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"
#include "EnvironmentQuery/Tests/EnvQueryTest_Overlap.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"

UEnvQueryTest_CustomOverlap::UEnvQueryTest_CustomOverlap(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ValidItemType = UEnvQueryItemType_VectorBase::StaticClass();
	SetWorkOnFloatValues(false);
}

FText UEnvQueryTest_CustomOverlap::GetDescriptionTitle() const { return FText::FromString("Custom Overlap Test"); }

void UEnvQueryTest_CustomOverlap::RunTest(FEnvQueryInstance& QueryInstance) const
{
	const TObjectPtr<UEnemyCommunicator> EnemyCommunicator = GetWorld()->GetSubsystem<UEnemyCommunicator>();
	const FCollisionShape CollisionShape = FCollisionShape::MakeBox(FVector(OverlapData.ExtentX, OverlapData.ExtentY, OverlapData.ExtentZ));
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		FVector Location = GetItemLocation(QueryInstance, It.GetIndex());
		const bool bHit = GetWorld()->OverlapAnyTestByChannel(Location, FQuat::Identity, ECC_Camera, CollisionShape);
		It.SetScore(TestPurpose, FilterType, bHit, BoolValue.GetValue());
		if (bHit) EnemyCommunicator->RemovePoint(It.GetIndex());
	}
}

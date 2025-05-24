#include "EnvQueryTest_CustomTrace.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"

UEnvQueryTest_CustomTrace::UEnvQueryTest_CustomTrace(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ValidItemType = UEnvQueryItemType_VectorBase::StaticClass();
	SetWorkOnFloatValues(false);
}

FText UEnvQueryTest_CustomTrace::GetDescriptionTitle() const { return FText::FromString("Custom Trace Test"); }

void UEnvQueryTest_CustomTrace::RunTest(FEnvQueryInstance& QueryInstance) const
{
	const TObjectPtr<UEnemyCommunicator> EnemyCommunicator = GetWorld()->GetSubsystem<UEnemyCommunicator>();
	const bool bTraceToItem = TraceFromContext.GetValue();
	const FVector QuerierLocation = Cast<AActor>(QueryInstance.Owner.Get())->GetActorLocation();
	const TObjectPtr<AActor> PlayerActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
    {
        FVector ItemLocation = GetItemLocation(QueryInstance, It.GetIndex());
        ItemLocation.Z +=  ItemHeightOffset.GetValue();
        FVector TraceStart = bTraceToItem ? QuerierLocation : ItemLocation;
        FVector TraceEnd = bTraceToItem ? ItemLocation : QuerierLocation;
        if (bTraceToItem) TraceStart.Z += ContextHeightOffset.GetValue();
        else TraceEnd.Z += ContextHeightOffset.GetValue();
        FHitResult HitResult;
        const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_WorldStatic);
    	if (PlayerActor)
    	{
		    const bool bHitPlayer = bHit && HitResult.GetActor() == PlayerActor;
    		It.SetScore(TestPurpose, FilterType, bHitPlayer, BoolValue.GetValue());
    		if (!bHitPlayer) EnemyCommunicator->RemovePoint(It.GetIndex());
    	}
	    else
	    {
	    	It.SetScore(TestPurpose, FilterType, bHit, BoolValue.GetValue());
	    	if (!bHit) EnemyCommunicator->RemovePoint(It.GetIndex());
	    }
    }
}

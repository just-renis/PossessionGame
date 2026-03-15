#include "AchievementManager.h"
#include "DelegateManager.h"
#include "OnlineStats.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineAchievementsInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "PossessionGame/Enums/EAchievement.h"

void UAchievementManager::Init(const TObjectPtr<UDelegateManager>& InDelegateManager)
{
	InDelegateManager->OnAchievementCompleted.BindUObject(this, &UAchievementManager::UnlockAchievement);
}

void UAchievementManager::UnlockAchievement(const EAchievement InAchievement)
{
	const IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
	if (!OSS) return;
	const IOnlineAchievementsPtr Achievements = OSS->GetAchievementsInterface();
	if (!Achievements.IsValid()) return;
	const IOnlineIdentityPtr Identity = OSS->GetIdentityInterface();
	if (!Identity.IsValid()) return;
	const TSharedPtr<const FUniqueNetId> PlayerId = Identity->GetUniquePlayerId(0);
	if (!PlayerId.IsValid()) return;
	const FString EnumString = UEnum::GetValueAsString(InAchievement);
	const FString AchievementID = EnumString.RightChop(EnumString.Find(TEXT("::")) + 2);
	Achievements->QueryAchievements(*PlayerId, FOnQueryAchievementsCompleteDelegate::CreateLambda([Achievements, AchievementID](const FUniqueNetId& PlayerId, bool bWasSuccessful)
	{
		if (bWasSuccessful)
		{
			FOnlineAchievementsWriteRef WriteObject = MakeShared<FOnlineAchievementsWrite>();
			WriteObject->SetFloatStat(AchievementID, 100.f);
			Achievements->WriteAchievements(PlayerId, WriteObject, FOnAchievementsWrittenDelegate());
		}
	}));
}

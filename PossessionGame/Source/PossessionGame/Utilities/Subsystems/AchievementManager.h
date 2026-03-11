#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AchievementManager.generated.h"

enum class EAchievement : uint8;
class UDelegateManager;

UCLASS()
class POSSESSIONGAME_API UAchievementManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	void Init(const TObjectPtr<UDelegateManager>& InDelegateManager);
private:
	void UnlockAchievement(const EAchievement InAchievement);
};

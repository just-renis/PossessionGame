#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/Generators/EnvQueryGenerator_Donut.h"
#include "EQG_AroundPlayer.generated.h"

enum class ECharacterType : uint8;

USTRUCT()
struct FDistanceRangeCharacters
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSet<ECharacterType> Values;
};

UCLASS()
class POSSESSIONGAME_API UEQG_AroundPlayer : public UEnvQueryGenerator
{
	GENERATED_BODY()
public:
	UEQG_AroundPlayer();
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TArray<float> DistanceRanges;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TMap<int32, FDistanceRangeCharacters> DistanceRangeCharacters;
	
	virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const override;
};

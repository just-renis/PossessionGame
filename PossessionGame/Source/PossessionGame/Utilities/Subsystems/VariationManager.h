#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "VariationManager.generated.h"

enum class ECharacterType : uint8;
enum class EVariation : uint8;
class USaveSlot;

UCLASS()
class POSSESSIONGAME_API UVariationManager : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	EVariation GetCurrentVariation(const ECharacterType InCharacterType) const;
	
	void Init(const TObjectPtr<USaveSlot>& InSaveSlot);
private:
	TObjectPtr<USaveSlot> SaveSlot;
};

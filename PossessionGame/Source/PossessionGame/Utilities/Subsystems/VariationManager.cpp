#include "VariationManager.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"

EVariation UVariationManager::GetCurrentVariation(const ECharacterType InCharacterType) const { return SaveSlot->GetSelectedVariation(InCharacterType); }

void UVariationManager::Init(const TObjectPtr<USaveSlot>& InSaveSlot) { SaveSlot = InSaveSlot; }

#include "PA_CharacterContainer.h"

FPrimaryAssetId UPA_CharacterContainer::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("Character"), CharacterName);
}
#pragma once

#include "Interactable.generated.h"

UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class POSSESSIONGAME_API IInteractable
{
	GENERATED_BODY()
public:
	virtual void Interact() = 0;
};

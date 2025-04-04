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
	bool GetIsInteractable() const { return bIsInteractable; }

	void SetIsInteractable(const bool bState) { bIsInteractable = bState; }
	virtual void Interact() = 0;
	virtual void ChangeOutlineState(const bool bState) = 0;
private:
	bool bIsInteractable = false;
};

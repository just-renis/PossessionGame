#pragma once

#include "Collectable.generated.h"

UINTERFACE(MinimalAPI)
class UCollectable : public UInterface
{
	GENERATED_BODY()
};

class POSSESSIONGAME_API ICollectable
{
	GENERATED_BODY()
public:
	virtual void Collect() = 0;
};

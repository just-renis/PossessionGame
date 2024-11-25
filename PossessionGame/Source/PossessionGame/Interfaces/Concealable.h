#pragma once

#include "Concealable.generated.h"

UINTERFACE(MinimalAPI)
class UConcealable : public UInterface
{
	GENERATED_BODY()
};

class IConcealable
{
	GENERATED_BODY()
public:
	virtual void SetConceal(bool bState) = 0;
};
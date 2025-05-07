#pragma once

#include "DamageReasonProvider.generated.h"

enum class EDamageReason : uint8;

UINTERFACE(MinimalAPI)
class UDamageReasonProvider : public UInterface
{
	GENERATED_BODY()
};

class POSSESSIONGAME_API IDamageReasonProvider
{
	GENERATED_BODY()
public:
	virtual EDamageReason GetDamageReason() const = 0;
};

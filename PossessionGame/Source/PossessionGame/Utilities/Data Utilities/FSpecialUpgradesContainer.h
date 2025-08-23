#pragma once

#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "FSpecialUpgradesContainer.generated.h"

USTRUCT()
struct FSpecialUpgradesContainer
{
	GENERATED_BODY()

	bool Contains(const EUpgradeAttribute InSpecialUpgradeAttribute) const { return SpecialAttributes.Contains(InSpecialUpgradeAttribute); }

	void Add(const EUpgradeAttribute InSpecialUpgradeAttribute) { SpecialAttributes.Add(InSpecialUpgradeAttribute); }
private:
	UPROPERTY()
	TSet<EUpgradeAttribute> SpecialAttributes;
};

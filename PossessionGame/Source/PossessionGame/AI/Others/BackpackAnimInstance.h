#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BackpackAnimInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API UBackpackAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	void SetIsOpen(const bool bState) { bIsOpen = bState; }
private:
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bIsOpen = false;
};

#pragma once

#include "CoreMinimal.h"
#include "Components/EditableText.h"
#include "CustomEditableText.generated.h"

UCLASS()
class POSSESSIONGAME_API UCustomEditableText : public UEditableText
{
	GENERATED_BODY()
public:
	void Init();
private:
	UFUNCTION()
	void OnTextChangedFiltered(const FText& InText);
};

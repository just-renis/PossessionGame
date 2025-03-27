#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ObjectiveMarker.generated.h"

class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UObjectiveMarker : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateMarkerText(const int32 InMeters) const;
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MarkerText;
};
